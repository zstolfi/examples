#pragma once
#include "math.hh"
#include <tuple>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <bit>
#include <initializer_list>
#include <optional>

constexpr struct FromPointCloud_Arg {} FromPointCloud {};
constexpr struct FromSimplex_Arg    {} FromSimplex    {};
constexpr struct FromRegular_Arg    {} FromRegular    {};

// https://en.wikipedia.org/wiki/Abstract_polytope
template <class Coord>
struct Polytope {
protected:
	std::list<Coord> coordinates {};

public:
	struct Face {
		int rank;
		std::set<Coord const*> coordRefs;
		auto operator<=>(Face const&) const = default;
		Face(decltype(rank) r=-1, decltype(coordRefs) cr={})
		: rank{r}, coordRefs{cr} {}

	private:
		friend struct Polytope;
		Polytope* parent = nullptr;
		Face(Polytope* that, decltype(rank) r=-1, decltype(coordRefs) cr={})
		: rank{r}, coordRefs{cr}, parent{that} {}

	public:
		// All points which touch 'this'.
		std::vector<Coord> points() const {
			std::vector<Coord> result {};
			for (Coord const* c : coordRefs) result.push_back(*c);
			return result;
		}

		// All sub-faces which belong to 'this'.
		std::vector<Face> lesserFacesOfRank(int otherRank) const {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(otherRank)) {
				if (stdr::includes(coordRefs, f.coordRefs)) {
					result.push_back(f);
				}
			}
			return result;
		}

		// All faces 'this' is a sub-face of.
		std::vector<Face> greaterFacesOfRank(int otherRank) const {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(otherRank)) {
				if (stdr::includes(f.coordRefs, coordRefs)) {
					result.push_back(f);
				}
			}
			return result;
		}

		std::vector<Face> neighbors() const {
			std::vector<Face> result {};
			std::vector<Face> ridges = lesserFacesOfRank(rank-1);
			for (Face const& f : parent->facesOfRank(rank)) {
				if (f == *this) continue;
				for (Face const& r : ridges) {
					if (stdr::includes(f.coordRefs, r.coordRefs)) {
						result.push_back(f);
						break;
					}
				}
			}
			return result;
		}

		// A simplex which lives entirely in 'this'.
		std::vector<Coord> simplex() const {
			std::vector<Coord> result {};
			for (Coord const* c : coordRefs) {
				result.push_back(*c);
				if (!independent(Affine, result)) result.pop_back();
				if (result.size() == rank+1) break;
			}
			assert(result.size() == rank+1);
			return result;
		}

		// Point which lives inside 'this', but not on the boundary.
		Coord inside() const {
			Coord sum {};
			for (Coord c : simplex()) sum += c;
			return sum / (rank+1);
		}

		// Same as inside(), but more symmetric and potentially expensive.
		Coord center() const {
			Coord sum {};
			for (Coord const* c : coordRefs) sum += *c;
			return sum / coordRefs.size();
		}

		// Basis coordinates for the space defined by 'this'.
		std::vector<Coord> frame() const {
			// TODO: Make the bases respect the RHR with the normal.
			return orthonormalize(Affine, simplex());
		};

		// Direction orthogonal to 'this' facing away from greater's center.
		Coord normal(Face const& greater) const {
			assert(rank < Coord::dimension);
			// We use a LUT of member function pointers to have our run-time
			// variable 'rank' act as a compile-time template parameter.
			return (this->*normal_tab[rank])(greater);
		}

	private:
		template <std::size_t K, std::size_t N=Coord::dimension>
		Coord normal_impl(Face const& greater) const {
			using SubCoord = Coord::template WithDimension<K+1>;
			// Make sure 'this' is a facet of greater.
			assert(greater.rank == K+1 && rank == K);
			assert(stdr::includes(greater.coordRefs, coordRefs));
			// This lower dimensional simplex will define our normal.
			std::vector<Coord> embeddedSimplex = simplex();
			// Matrices for projecting downwards, then upwards.
			Matrix<typename Coord::value_type, K+1, N> matrix {greater.frame()};
			// Project downwards.
			std::vector<SubCoord> projectedSimplex {};
			for (Coord c : embeddedSimplex) {
				projectedSimplex.push_back(matrix * c);
			}
			// Find orthogonal.
			SubCoord projectedResult = orthogonal(Affine, projectedSimplex);
			// Project back up.
			Coord result = matrix.transpose() * projectedResult;
			result /= length(result);
			// Point away from greater's center.
			bool facingInside = dot(result, inside()-greater.inside()) < 0;
			return facingInside? -result: result;
		}

		// Instantiate normal_impl<0> ... normal_impl<N-1>.
		static constexpr auto normal_tab =
			[]<std::size_t ... I>(std::index_sequence<I ... >) {
				return std::array {&Face::normal_impl<I> ... };
			} (std::make_index_sequence<Coord::dimension> {})
		;
	};

	std::set<Face> faces {
		{this, -1, {}} // Every abstract polytope has the emtpy set.
	};

	Polytope() = default;

	std::vector<Face> facesOfRank(int rank) const {
		return {
			faces.lower_bound({rank}),
			faces.upper_bound({rank+1}),
		};
	}

//	// Equivalent to facesOfRank(k).size().
//	std::size_t faceCountOfRank(int rank) const {
//		return stdr::distance(
//			faces.lower_bound({rank}),
//			faces.upper_bound({rank+1})
//		);
//	}

	// Every non-compound polytope will only have 1 face of least/greatest rank.
	Face const& least   () const { return *faces. begin(); }
	Face const& greatest() const { return *faces.rbegin(); }

/* ~~ Constructors ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

	// Unassembled collection of points (DIY).
	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromPointCloud_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		for (Coord const& c : coordinates) {
			faces.insert({this, 0, {&c}});
		}
	}

	// Construct a possibly degenerate simplex.
	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromSimplex_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		// Iterate power set of input.
		for (unsigned bits=0; bits<(1<<coordinates.size()); bits++) {
			std::set<Coord const*> subset {};
			for (unsigned i=0; Coord const& c : coordinates) {
				if ((bits >> i++) & 1) subset.insert(&c);
			}
			faces.insert({this, std::popcount(bits)-1, subset});
		}
	}

	// Assume our points are evenly spaced to their closest neighbors.
	// TODO: Allow this algorithm to work with compound polytopes.
	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromRegular_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		using T = Coord::value_type;
		for (Coord const& c : coordinates) {
			faces.insert({this, 0, {&c}});
			// Sort all points by distance to c.
			std::multimap<T, Coord const*> distanceOrder {};
			for (Coord const& d : coordinates) {
				distanceOrder.insert({distanceSquared(c, d), &d});
			}
			// Insert edge.
			T minDistance = stdr::next(distanceOrder.begin())->first;
			stdr::for_each(
				distanceOrder.lower_bound(minDistance),
				distanceOrder.upper_bound(minDistance + Epsilon<T>),
				[&](auto pair) { faces.insert({this, 1, {&c, pair.second}}); }
			);
		}
		// Every k-face is a set of at least k (k-1)-faces on the convex hull.
		std::vector<Face> kFaces {};
		for (int k=1; (kFaces=facesOfRank(k)).size() > k; k++) {
			for (Face f1 : kFaces) { 
				for (Face f2 : f1.neighbors()) {
					if (auto possibleGreater = reachable(f1, f2)) {
						faces.insert(*possibleGreater);
					}
				}
			}
			if (k+1 == Coord::dimension) {
				std::set<Coord const*> all {};
				for (Coord const& c : coordinates) all.insert(&c);
				faces.insert({this, Coord::dimension, all});
				break;
			}
		}
	}

private:
	// Returns the face of rank k+1 in the affine plane defined by f1 and f2,
	// which only contains reachable faces. Might not be on the convex hull.
	std::optional<Face> reachable(Face const& f1, Face const& f2) {
		assert(f1.rank == f2.rank);
		int k = f1.rank;
		std::set<Coord const*> seenCoords = unite(f1.coordRefs, f2.coordRefs);
		std::vector<Coord> planePoints = Face{k+1, seenCoords}.simplex();
		// Isolate all the faces which lie entirely on our plane of interest.
		std::set<Face> possible {}, seenFaces {};
		for (Face f : facesOfRank(k)) {
			bool onPlane = stdr::all_of(f.coordRefs, [&](Coord const* c) {
				if (k+1 == Coord::dimension) return true;
				return !independent(Affine, planePoints, *c);
			});
			if (onPlane) possible.insert(f);
		}
		assert(possible.contains(f1) && possible.contains(f2));
		// Survey the reachable faces while building up a graph.
		std::map<Face, std::set<Face>> graph {};
		// Breadth first search.
		for (
			std::set<Face> horizon {f1}, newHorizon {};
			!horizon.empty();
			horizon = newHorizon, newHorizon = {}
		) {
			for (Face f : horizon) {
				// Store the points for our possible (k+1)-face.
				for (Coord const* c : f.coordRefs) seenCoords.insert(c);
				seenFaces.insert(f);
				// Expand our horizon.
				for (Face n : f.neighbors()) {
					if (possible.contains(n) && !seenFaces.contains(n)) {
						graph[f].insert(n);
						graph[n].insert(f);
						newHorizon.insert(n);
					}
				}
			}
		}
		if (graph.size() < k+1) return std::nullopt;
		auto disconnect = [&](auto pair) { return pair.second.size() <= k; };
		if (stdr::any_of(graph, disconnect)) return std::nullopt;
		return Face {this, k+1, seenCoords};
	}
};

template <stdr::input_range R>
Polytope(FromPointCloud_Arg, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Coord>
Polytope(FromPointCloud_Arg, std::initializer_list<Coord>)
-> Polytope<Coord>;

template <stdr::input_range R>
Polytope(FromSimplex_Arg, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Coord>
Polytope(FromSimplex_Arg, std::initializer_list<Coord>)
-> Polytope<Coord>;

template <stdr::input_range R>
Polytope(FromRegular_Arg, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Coord>
Polytope(FromRegular_Arg, std::initializer_list<Coord>)
-> Polytope<Coord>;

#pragma once
#include "math.hh"
#include <set>
#include <map>
#include <vector>
#include <bit>
#include <initializer_list>
#include <optional>

constexpr struct FromPointCloud_Arg {} FromPointCloud {};
constexpr struct FromSimplex_Arg    {} FromSimplex    {};
constexpr struct FromRegular_Arg    {} FromRegular    {};

// https://en.wikipedia.org/wiki/Abstract_polytope
template <class Coord>
class Polytope {
	std::vector<Coord> coordinates {};

public:
	class Face {
		friend class Polytope;
		Polytope const* parent;

		int m_rank {-1};
		std::set<std::size_t> m_indices {};
		auto get(auto i) const { return parent->coordinates[i]; }

		Face() = delete;
		Face(Polytope const* p, int r=-1, std::set<std::size_t> i={})
		: parent{p}, m_rank{r}, m_indices{i} {}

	public:
		auto operator<=>(Face const&) const = default;

		// Dimension of 'this'.
		int rank() const { return m_rank; }

		// All points which touch 'this'.
		std::vector<Coord> points() const {
			std::vector<Coord> result {};
			for (auto i : m_indices) result.push_back(get(i));
			return result;
		}

		// All sub-faces which belong to 'this'.
		std::vector<Face> lesserFacesOfRank(int otherRank) const {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(otherRank)) {
				if (stdr::includes(m_indices, f.m_indices)) {
					result.push_back(f);
				}
			}
			return result;
		}

		// All faces 'this' is a sub-face of.
		std::vector<Face> greaterFacesOfRank(int otherRank) const {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(otherRank)) {
				if (stdr::includes(f.m_indices, m_indices)) {
					result.push_back(f);
				}
			}
			return result;
		}

	private: // May replace with closure, star, and link.
		std::vector<Face> neighbors() const {
			std::vector<Face> result {};
			std::vector<Face> ridges = lesserFacesOfRank(m_rank-1);
			for (Face const& f : parent->facesOfRank(m_rank)) {
				if (f == *this) continue;
				for (Face const& r : ridges) {
					if (stdr::includes(f.m_indices, r.m_indices)) {
						result.push_back(f);
						break;
					}
				}
			}
			return result;
		}
	public:

		// A simplex which lives entirely in 'this'.
		std::vector<Coord> simplex() const {
			std::vector<Coord> result {};
			for (auto i : m_indices) {
				result.push_back(get(i));
				if (!independent(Affine, result)) result.pop_back();
				if (result.size() == m_rank+1) break;
			}
			assert(result.size() == m_rank+1);
			return result;
		}

		// Point which lives inside 'this', but not on the boundary.
		Coord inside() const {
			Coord sum {};
			for (Coord c : simplex()) sum += c;
			return sum / (m_rank+1);
		}

		// Same as inside(), but more symmetric and potentially expensive.
		Coord center() const {
			Coord sum {};
			for (auto i : m_indices) sum += get(i);
			return sum / m_indices.size();
		}

		// Basis coordinates for the space defined by 'this'.
		std::vector<Coord> frame() const {
			// TODO: Make the bases respect the RHR with the normal.
			return orthonormalize(Affine, simplex());
		};

		// Direction orthogonal to 'this' facing away from greater's center.
		Coord normal(Face const& greater) const {
			assert(m_rank < Coord::dimension);
			// We use a LUT of member function pointers to have our run-time
			// variable 'rank' act as a compile-time template parameter.
			return (this->*normal_tab[m_rank])(greater);
		}

	private:
		template <std::size_t K, std::size_t N=Coord::dimension>
		Coord normal_impl(Face const& greater) const {
			using SubCoord = Coord::template WithDimension<K+1>;
			// Make sure 'this' is a facet of greater.
			assert(greater.m_rank == K+1 && m_rank == K);
			assert(stdr::includes(greater.m_indices, m_indices));
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

	Polytope(Polytope const& other)
	: coordinates{other.coordinates.begin(), other.coordinates.end()} {
		for (Face f : other.faces) {
			f.parent = this;
			faces.insert(f);
		}
	}

	Polytope(Face const& other) {
		assert(other.parent != this);
		std::vector<Coord> const* oldCoordinates {&other.parent->coordinates};
		std::map<std::size_t, std::size_t> indexMap {};
		for (auto i : other.m_indices) {
			indexMap[i] = coordinates.size();
			coordinates.push_back((*oldCoordinates)[i]);
		}
		for (int k=-1; k<=other.m_rank; k++) {
			for (Face const& f : other.lesserFacesOfRank(k)) {
				std::set<std::size_t> indexSet {};
				for (auto i : f.m_indices) indexSet.insert(indexMap[i]);
				faces.insert({this, k, indexSet});
			}
		}
	}

	Polytope& transform(auto&& f) {
		for (Coord& c : coordinates) c = f((Coord const&)c);
		return *this;
	}

	std::vector<Face> facesOfRank(int rank) const {
		return {
			faces.lower_bound({this, rank}),
			faces.upper_bound({this, rank+1}),
		};
	}

//	// Equivalent to facesOfRank(k).size().
//	std::size_t faceCountOfRank(int rank) const {
//		return stdr::distance(
//			faces.lower_bound({this, rank}),
//			faces.upper_bound({this, rank+1})
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
		for (std::size_t i=0; i<coordinates.size(); i++) {
			faces.insert({this, 0, {i}});
		}
	}

	// Construct a possibly degenerate simplex.
	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromSimplex_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		// Iterate power set of input.
		for (unsigned bits=0; bits<(1<<coordinates.size()); bits++) {
			std::set<std::size_t> subset {};
			for (std::size_t i=0; i<coordinates.size(); i++) {
				if ((bits >> i) & 1) subset.insert(i);
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
		for (std::size_t i=0; i<coordinates.size(); i++) {
			faces.insert({this, 0, {i}});
			// Sort all points by distance to c.
			std::multimap<T, std::size_t> distanceOrder {};
			for (std::size_t j=0; j<coordinates.size(); j++) {
				T dist = distanceSquared(coordinates[i], coordinates[j]);
				distanceOrder.insert({dist, j});
			}
			// Insert edge.
			T minDistance = stdr::next(distanceOrder.begin())->first;
			stdr::for_each(
				distanceOrder.lower_bound(minDistance),
				distanceOrder.upper_bound(minDistance + Epsilon<T>),
				[&](auto pair) { faces.insert({this, 1, {i, pair.second}}); }
			);
		}
		// Every k-face is a set of at least k (k-1)-faces on the convex hull.
		std::vector<Face> kFaces {};
		for (int k=1; (kFaces=facesOfRank(k)).size(); k++) {
			// If we have all our facets, creating the greatest face is trivial.
			if (k == Coord::dimension) {
				std::set<std::size_t> all {};
				for (std::size_t i=0; i<coordinates.size(); i++) all.insert(i);
				faces.insert({this, Coord::dimension, all});
			}
			// Remove all (k-1)-faces which don't lie on the convex hull that
			// our previous iteration discovered.
			if (k > 2) for (Face f : kFaces) {
				for (Face g : f.lesserFacesOfRank(k-1)) {
					Coord normal = g.normal(f);
					Coord inside = g.inside();
					auto vertices = onPlane(Affine, 0, f);
					int count = 0;
					bool removable = stdr::any_of(vertices, [&](auto v) {
						std::size_t index = *v.m_indices.begin();
						if (g.m_indices.contains(index)) return false;
						Coord c = coordinates[index];
						int sign = dot(c - inside, normal) < 0? -1: 1;
						if (count > 0 && sign < 0) return true;
						if (count < 0 && sign > 0) return true;
						count += sign; return false;
					});
					if (removable) faces.erase(g);
				}
			}
			// Find (k+1)-faces which lie on the hyperplane defined by two
			// neighboring k-faces. If the polytope is non-elementary we will
			// generate more faces than the convex hull has.
			for (Face f1 : kFaces) { 
				for (Face f2 : f1.neighbors()) {
					if (auto discovery = dicoverGreaterFace(f1, f2)) {
						faces.insert(*discovery);
					}
				}
			}
		}
	}

private:
	// Returns the face of rank k+1 in the affine plane defined by f1 and f2,
	// which only contains reachable faces. Might not be on the convex hull.
	std::optional<Face> dicoverGreaterFace(Face const& f1, Face const& f2) {
		assert(f1.m_rank == f2.m_rank);
		int k = f1.m_rank;
		std::set<std::size_t> seenCoords = unite(f1.m_indices, f2.m_indices);
		// Bail out if this face is already known.
		if (stdr::any_of(facesOfRank(k+1), [&](Face const& f) {
			return stdr::includes(f.m_indices, seenCoords);
		})) return std::nullopt;
		// Isolate all the faces which lie entirely on our plane of interest.
		auto possible = onPlane(Affine, k, Face{this, k+1, seenCoords});
		assert(possible.contains(f1) && possible.contains(f2));
		// Survey the reachable faces while building up a graph.
		std::map<Face, std::set<Face>> graph {};
		// Breadth first search.
		for (
			std::set<Face> horizon {f1}, newHorizon {}, seenFaces {};
			!horizon.empty();
			horizon = newHorizon, newHorizon = {}
		) {
			for (Face f : horizon) {
				// Store the points for our possible (k+1)-face.
				for (auto i : f.m_indices) seenCoords.insert(i);
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

	std::set<Face> onPlane(Affine_Arg, int rank, Face const& plane) {
		assert(rank < plane.m_rank);
		std::set<Face> result {};
		auto const simplex = plane.simplex();
		for (Face const& f : facesOfRank(rank)) {
			bool onPlane = stdr::all_of(f.m_indices, [&](std::size_t i) {
				if (plane.m_rank == Coord::dimension) return true;
				return !independent(Affine, simplex, coordinates[i]);
			});
			if (onPlane) result.insert(f);
		}
		return result;
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

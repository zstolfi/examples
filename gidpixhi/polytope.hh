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
constexpr struct FromConvexHull_Arg {} FromConvexHull {};
constexpr struct FromRegular_Arg    {} FromRegular    {};

// https://en.wikipedia.org/wiki/Abstract_polytope
template <class Coord>
class Polytope {
	std::vector<Coord> coordinates {};
	using CoordT = Coord::value_type;
	static constexpr auto CoordN = Coord::dimension;

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
			if (otherRank == m_rank) return {*this};
			if (otherRank > m_rank) return {};
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
			if (otherRank == m_rank) return {*this};
			if (otherRank < m_rank) return {};
			for (Face const& f : parent->facesOfRank(otherRank)) {
				if (stdr::includes(f.m_indices, m_indices)) {
					result.push_back(f);
				}
			}
			return result;
		}

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
			assert(m_rank < CoordN);
			// We use a LUT of member function pointers to have our run-time
			// variable 'rank' act as a compile-time template parameter.
			return (this->*normal_tab[m_rank])(greater);
		}

	private:
		template <std::size_t K>
		Coord normal_impl(Face const& greater) const {
			constexpr std::size_t N = CoordN;
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
			} (std::make_index_sequence<CoordN> {})
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
		createSimplex(coordinates.size());
	}

	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromRegular_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		Polytope(FromConvexHull, input);
	}

	// Assume all input points lie on the final hull.
	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromConvexHull_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		// Partially sort our coordinates so that a simplex of the highest
		// possible rank comes first.
		int hullRank = simplexFirst(coordinates);
		createConvexHull(hullRank, stdv::iota(0u, coordinates.size()));
	}

private:
	void createConvexHull(int rank, stdr::input_range auto&& available) {
		Face hull = createSimplex(available | stdv::take(rank+1));
		for (std::size_t eye : available | stdv::drop(rank+1)) {
			faces.insert({this, 0, {eye}});
			// 'visible' and 'horizon' contain faces of all lower ranks.
			// 'coplanar' faces, if any, require lower dimensional recursion.
			auto const& [visible, horizon, coplanar]
			= findHorizon(hull, coordinates[eye]);
			for (Face const& f : horizon) {
				// Construct ring of pyramids.
				faces.insert({this, f.m_rank+1, unite(f.m_indices, {eye})});
			}
			// Everything inside our horizon is invisible now.
			deleteFaces(visible);
			// Coplanar facets are non-trivial if you want non-simplicial edges.
			for (Face const& f : coplanar) {
				std::vector<std::size_t> indices {eye};
				for (std::size_t i : f.m_indices) indices.push_back(i);
				// May need to remove all descendants of 'f' as well.
				int subRank = simplexFirst(indices, rank-1);
				assert(subRank == rank-1);
				createConvexHull(rank-1, indices);
			}
			// Update our hull as it appears in 'faces'.
			faces.erase(hull);
			hull.m_indices.insert(eye);
			faces.insert(hull);
		}
	}

	Face const& createSimplex(auto const& indices) {
		assert(indices.size() < 8*sizeof(unsigned));
		auto returnVal = faces.begin();
		// Iterate power set of input.
		unsigned bits = 1 << indices.size();
		while (bits--) {
			std::set<std::size_t> subset {};
			for (std::size_t i=0; i<indices.size(); i++) {
				if ((bits >> i) & 1) subset.insert(indices[i]);
			}
			auto ir = faces.insert({this, std::popcount(bits)-1, subset});
			if (subset.size() == indices.size()) returnVal = ir.first;
		}
		return *returnVal;
	}

	template <stdr::random_access_range R>
	requires(std::same_as<stdr::range_value_t<R>, Coord>)
	int simplexFirst(R& points, int maxRank=CoordN) {
		int rank = -1;
		for (std::size_t i=0; i<points.size(); i++) {
			std::swap(points[rank+1], points[i]);
			if (independent(Affine, points | stdv::take(rank+2))) rank++;
			if (rank == maxRank) break;
		}
		return rank;
	}

	template <stdr::random_access_range R>
	requires(std::same_as<stdr::range_value_t<R>, std::size_t>)
	int simplexFirst(R& indices, int maxRank=CoordN) {
		std::vector<Coord> coords {};
		int rank = -1;
		for (std::size_t i=0; i<indices.size(); i++) {
			std::swap(indices[rank+1], indices[i]);
			coords.push_back(coordinates[indices[i]]);
			if (independent(Affine, coords)) rank++;
			else coords.pop_back();
			if (rank == maxRank) break;
		}
		return rank;
	}

	auto findHorizon(Face const& hull, Coord const& eye) {
		// TODO: Make this breadth first search.
		std::set<Face> seen {}, coplanar {};
		for (Face const& facet : hull.lesserFacesOfRank(hull.m_rank-1)) {
			auto distance = dot(eye - facet.inside(), facet.normal(hull));
			bool onPlane = std::abs(distance) < Epsilon<CoordT>;
			if (onPlane || distance > 0) {
				for (Face const& f : faces) {
					if (stdr::includes(facet.m_indices, f.m_indices)) {
						seen.insert(f);
						if (onPlane) coplanar.insert(f);
					}
				}
			}
		}
		// Start with constructing our facets.
		std::set<Face> visible {
			seen.lower_bound({this, hull.m_rank-1}),
			seen.end()
		};
		// All horizon faces touch a facet not belonging to 'visible'.
		std::set<Face> horizon {};
		for (Face const& f : seen) {
			auto onBoundary = !coplanar.contains(f) && !stdr::all_of(
				f.greaterFacesOfRank(hull.m_rank-1),
				[&](Face const& g) { return visible.contains(g); }
			);
			(onBoundary? horizon: visible).insert(f);
		}
		std::set<Face> coplanarFacets {
			coplanar.lower_bound({this, hull.m_rank-1}),
			coplanar.end()
		};
		return std::tuple {visible, horizon, coplanarFacets};
	}

	void deleteFaces(stdr::input_range auto&& faceSet) {
		for (Face const& f : faceSet) {
			faces.erase(f);
		}
		faces.insert({this, -1, {}}); // The empty face is eternal.
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

template <stdr::input_range R>
Polytope(FromConvexHull_Arg, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Coord>
Polytope(FromConvexHull_Arg, std::initializer_list<Coord>)
-> Polytope<Coord>;

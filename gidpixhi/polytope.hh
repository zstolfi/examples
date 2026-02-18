#pragma once
#include "math.hh"
#include <tuple>
#include <set>
#include <list>
#include <vector>
#include <bit>
#include <initializer_list>

constexpr struct FromPointCloud_Arg {} FromPointCloud {};

// https://en.wikipedia.org/wiki/Abstract_polytope
template <class Coord>
struct Polytope {
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
		std::vector<Face> lesserFacesOfRank(int rank) {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(rank)) {
				if (stdr::includes(coordRefs, f.coordRefs)) {
					result.push_back(f);
				}
			}
			return result;
		}

		// All faces 'this' is a sub-face of.
		std::vector<Face> greaterFacesOfRank(int rank) {
			std::vector<Face> result {};
			for (Face const& f : parent->facesOfRank(rank)) {
				if (stdr::includes(f.coordRefs, coordRefs)) {
					result.push_back(f);
				}
			}
			return result;
		}

		// A simplex which lives entirely in 'this'.
		std::vector<Coord> simplex() const {
			std::vector<Coord> result (rank+1);
			std::size_t i = 0;
			for (i=0; Coord const* c : coordRefs) {
				result[i] = *c;
				if (1 /*affinelyIndependent(i, result | stdv::take(i+1))*/) {
					if (++i == rank+1) break;
				}
			}
			assert(i == rank+1);
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
			return orthonormalize(FromSimplex, simplex());
		};

		// Direction orthogonal to 'this' facing away from the parent's center.
		Coord normal(Face const& parent) const {
			// This will be the most complicated to compute. We'll need to
			// traverse the flag defined by the greatest face and the parent.
			// We'll keep track of 'this's simplex as a set of k+1 N-D vectors,
			// with each projection downards we lose a dimension until we have
			// k+1 points in k+1 dimensions living on the parent's space. From
			// there we find the normal using the k+1 analog of the cross
			// product, projecting upwards after until we get out desired N-D
			// vector (up to inversion).
			return {/* TODO */};
		}
	};

	std::set<Face> faces {
		{this, -1, {}} // Every abstract polytope has the emtpy set.
	};

	Polytope() = default;

	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromPointCloud_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		for (Coord const& c : coordinates) {
			faces.insert({this, 0, {&c}});
		}
	}

	template <stdr::input_range R=std::initializer_list<Coord>>
	Polytope(FromSimplex_Arg, R&& input)
	: coordinates{stdr::begin(input), stdr::end(input)} {
		for (unsigned bits=0; bits<(1<<coordinates.size()); bits++) {
			std::set<Coord const*> subset {};
			for (unsigned i=0; Coord const& c : coordinates) {
				if ((bits >> i++) & 1) subset.insert(&c);
			}
			faces.insert({this, std::popcount(bits)-1, subset});
		}
	}

	std::vector<Face> facesOfRank(int rank) const {
		return {
			faces.lower_bound({rank}),
			faces.upper_bound({rank+1}),
		};
	}

	// Every non-compound polytope will only have 1 face of least/greatest rank.
	Face const& least   () const { return *faces. begin(); }
	Face const& greatest() const { return *faces.rbegin(); }

protected:
	std::list<Coord> coordinates {};
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

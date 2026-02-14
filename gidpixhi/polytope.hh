#pragma once
#include <set>
#include <list>
#include <bit>
#include <initializer_list>
#include <ranges>
namespace stdr = std::ranges;
namespace stdv = std::views;

constexpr struct FromPointCloud_Arg {} FromPointCloud {};
constexpr struct FromSimplex_Arg    {} FromSimplex    {};

// https://en.wikipedia.org/wiki/Abstract_polytope
template <class Point>
struct Polytope {
	struct Face {
		int rank;
		std::set<Point const*> points;
		auto operator<=>(Face const&) const = default;
	};
	
	std::set<Face> faces {
		{-1, {}} // Every abstract polytope has the emtpy set.
	};

	template <stdr::input_range R=std::initializer_list<Point>>
	Polytope(FromPointCloud_Arg, int rank, R&& input)
	: m_points{stdr::begin(input), stdr::end(input)} {
		std::set<Point const*> all {};
		for (Point const& p : m_points) all.insert(&p);
		faces.insert({rank, all});
	}

	template <stdr::input_range R=std::initializer_list<Point>>
	Polytope(FromSimplex_Arg, R&& input)
	: m_points{stdr::begin(input), stdr::end(input)} {
		for (unsigned bits=0; bits<(1<<m_points.size()); bits++) {
			std::set<Point const*> subset {};
			for (unsigned i=0; Point const& p : m_points) {
				if ((bits >> i++) & 1) subset.insert(&p);
			}
			faces.insert({std::popcount(bits)-1, subset});
		}
	}

	struct FaceRange {
		std::set<Face>::iterator m_begin {}, m_end {};
		auto begin() { return m_begin; }
		auto end() { return m_end; }
	};

	FaceRange getFaces(int rank) const {
		return FaceRange {
			faces.lower_bound({rank}),
			faces.upper_bound({rank+1}),
		};
	}

protected:
	std::list<Point> m_points {};
};

template <stdr::input_range R>
Polytope(FromPointCloud_Arg, int, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Point>
Polytope(FromPointCloud_Arg, int, std::initializer_list<Point>)
-> Polytope<Point>;

template <stdr::input_range R>
Polytope(FromSimplex_Arg, R)
-> Polytope<stdr::range_value_t<R>>;

template <class Point>
Polytope(FromSimplex_Arg, std::initializer_list<Point>)
-> Polytope<Point>;

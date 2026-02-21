#pragma once
#include "polytope.hh"
#include <stdexcept>
#include <format>
#include <numbers>
using std::numbers::phi;

namespace Johnson {
	Polytope<Coordinate<double, 3>> const
	// J1.
	SquarePyramid {
		FromRegular, {
			{+1, 0, 0},
			{-1, 0, 0},
			{0, +1, 0},
			{0, -1, 0},
			{0, 0, 1},
		}
	},
	// J2.
	PentagonalPyramid {
		FromRegular, {
			{+1, 0, phi},
			{-1, 0, phi},
			{0, +phi, 1},
			{0, -phi, 1},
			{phi, +1, 0},
			{phi, -1, 0},
		}
	};

	auto const& J(std::size_t n) {
		if (n == 1) return SquarePyramid;
		if (n == 2) return PentagonalPyramid;
		throw std::out_of_range {
			std::format("Invalid Johnson Solid number, {}.", n)
		};
	}
};

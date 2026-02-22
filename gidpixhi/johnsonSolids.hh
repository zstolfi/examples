#pragma once
#include "polytope.hh"
#include <stdexcept>
#include <format>
#include <numbers>
using std::numbers::phi;

namespace Johnson {
	Polytope<Coordinate<double, 3>> const
	// J1: https://en.wikipedia.org/wiki/Equilateral_square_pyramid
	SquarePyramid {
		FromRegular, {
			{+1, 0, 0},
			{-1, 0, 0},
			{0, +1, 0},
			{0, -1, 0},
			{0, 0, 1},
		}
	},
	// J2: https://en.wikipedia.org/wiki/Pentagonal_pyramid#As_a_Johnson_solid
	PentagonalPyramid {
		FromRegular, {
			{+1, 0, phi},
			{-1, 0, phi},
			{0, +phi, 1},
			{0, -phi, 1},
			{phi, +1, 0},
			{phi, -1, 0},
		}
	},
	// J3: https://en.wikipedia.org/wiki/Triangular_cupola
	TriangularCupola {
		FromRegular, {
			{1, 1, 0},
			{1, 0, 1},
			{0, 1, 1},
			{-1, +1, 0},
			{+1, -1, 0},
			{-1, 0, +1},
			{+1, 0, -1},
			{0, -1, +1},
			{0, +1, -1},
		}
	},
	// J4: https://en.wikipedia.org/wiki/Square_cupola
	SquareCupola {
		FromRegular, {
			{+(1+sqrt(2)), +1, +1},
			{+(1+sqrt(2)), -1, +1},
			{-(1+sqrt(2)), +1, +1},
			{-(1+sqrt(2)), -1, +1},
			{+1, +(1+sqrt(2)), +1},
			{+1, -(1+sqrt(2)), +1},
			{-1, +(1+sqrt(2)), +1},
			{-1, -(1+sqrt(2)), +1},
			{+1, +1, +(1+sqrt(2))},
			{+1, -1, +(1+sqrt(2))},
			{-1, +1, +(1+sqrt(2))},
			{-1, -1, +(1+sqrt(2))},
		}
	},
	// J5: https://en.wikipedia.org/wiki/Pentagonal_cupola
	PentagonalCupola {
		FromRegular, {
			{+1, +1, +phi*phi*phi},
			{+1, -1, +phi*phi*phi},
			{-1, +1, +phi*phi*phi},
			{-1, -1, +phi*phi*phi},
			{+phi*phi*phi, +1, +1},
			{+phi*phi*phi, -1, +1},
			{+phi*phi, +phi, +2*phi},
			{+phi*phi, -phi, +2*phi},
			{+phi, +2*phi, +phi*phi},
			{+phi, -2*phi, +phi*phi},
			{+2*phi, +phi*phi, +phi},
			{+2*phi, -phi*phi, +phi},
			{+(2+phi), 0, +phi*phi},
			{0, +phi*phi, +(2+phi)},
			{0, -phi*phi, +(2+phi)},
		}
	},

	PentagonalRotunda {
		FromRegular, {
			{+2, 0, 0},
			{0, 0, +2},
			{0, +2, 0},
			{0, -2, 0},
			{+phi, +1/phi, +1},
			{+phi, +1/phi, -1},
			{+phi, -1/phi, +1},
			{+phi, -1/phi, -1},
			{-phi, +1/phi, +1},
			{-phi, -1/phi, +1},
			{+1/phi, +1, +phi},
			{+1/phi, -1, +phi},
			{-1/phi, +1, +phi},
			{-1/phi, -1, +phi},
			{+1, +phi, +1/phi},
			{-1, +phi, +1/phi},
			{+1, +phi, -1/phi},
			{+1, -phi, +1/phi},
			{-1, -phi, +1/phi},
			{+1, -phi, -1/phi},
		}
	};

	auto const& J(std::size_t n) {
		if (n == 1) return SquarePyramid;
		if (n == 2) return PentagonalPyramid;
		if (n == 3) return TriangularCupola;
		if (n == 4) return SquareCupola;
		if (n == 5) return PentagonalCupola;
		if (n == 6) return PentagonalRotunda;
		throw std::out_of_range {
			std::format("Invalid Johnson Solid number, {}.", n)
		};
	}
};

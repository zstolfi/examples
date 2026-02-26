#pragma once
#include "polytope.hh"
#include <stdexcept>
#include <format>
#include <numbers>
using std::numbers::phi;

namespace Johnson {
	Polytope<Coordinate<double, 3>> const
	// J1: https://en.wikipedia.org/wiki/Square_pyramid
	SquarePyramid {
		FromRegular, {
			{+1, 0, 0},
			{-1, 0, 0},
			{0, +1, 0},
			{0, -1, 0},
			{0, 0, 1},
		}
	},
	// J2: https://en.wikipedia.org/wiki/Pentagonal_pyramid
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
	// J6: https://en.wikipedia.org/wiki/Pentagonal_rotunda
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
	},
	// J7: https://en.wikipedia.org/wiki/Elongated_triangular_pyramid
	ElongatedTriangularPyramid {
		FromRegular, {
			{2*sqrt(3)/3, 0, +1},
			{2*sqrt(3)/3, 0, -1},
			{-sqrt(3)/3, +1.0, +1},
			{-sqrt(3)/3, +1.0, -1},
			{-sqrt(3)/3, -1.0, +1},
			{-sqrt(3)/3, -1.0, -1},
			{0, 0, 1 + 2*sqrt(6)/3},
		}
	},
	// J8: https://en.wikipedia.org/wiki/Elongated_square_pyramid
	ElongatedSquarePyramid {
		FromRegular, {
			{+1, +1, +1},
			{+1, +1, -1},
			{+1, -1, +1},
			{+1, -1, -1},
			{-1, +1, +1},
			{-1, +1, -1},
			{-1, -1, +1},
			{-1, -1, -1},
			{0, 0, 1 + sqrt(2)},
		}
	},
	// J9: https://en.wikipedia.org/wiki/Elongated_pentagonal_pyramid
	ElongatedPentagonalPyramid {
		FromRegular, {
			{sqrt(50+10*sqrt(5))/5, 0, +1},
			{sqrt(50+10*sqrt(5))/5, 0, -1},
			{+sqrt(2/(5+sqrt(5))), +phi, +1},
			{+sqrt(2/(5+sqrt(5))), +phi, -1},
			{-sqrt((5+2*sqrt(5))/5), +1, +1},
			{-sqrt((5+2*sqrt(5))/5), +1, -1},
			{-sqrt((5+2*sqrt(5))/5), -1, +1},
			{-sqrt((5+2*sqrt(5))/5), -1, -1},
			{+sqrt(2/(5+sqrt(5))), -phi, +1},
			{+sqrt(2/(5+sqrt(5))), -phi, -1},
			{0, 0, 1 + sqrt((10-2*sqrt(5))/5)},
		}
	};

	auto const& J(std::size_t n) {
		if (n == 1) return SquarePyramid;
		if (n == 2) return PentagonalPyramid;
		if (n == 3) return TriangularCupola;
		if (n == 4) return SquareCupola;
		if (n == 5) return PentagonalCupola;
		if (n == 6) return PentagonalRotunda;
		if (n == 7) return ElongatedTriangularPyramid;
		if (n == 8) return ElongatedSquarePyramid;
		if (n == 9) return ElongatedPentagonalPyramid;
		throw std::out_of_range {
			std::format("Invalid Johnson Solid number, {}.", n)
		};
	}
}

using XYZ = Coordinate<double, 3>;
using XYZW = Coordinate<double, 4>;
Polytope const cuboctahedralPyramid {
	FromRegular, std::vector<XYZW> {
		{-1,  0, +1,  0},
		{ 0, -1, +1,  0},
		{+1,  0, +1,  0},
		{ 0, +1, +1,  0},
		{-1, +1,  0,  0},
		{-1, -1,  0,  0},
		{+1, -1,  0,  0},
		{+1, +1,  0,  0},
		{-1,  0, -1,  0},
		{ 0, -1, -1,  0},
		{+1,  0, -1,  0},
		{ 0, +1, -1,  0},
		{ 0,  0,  0, -1}
	},
};

Polytope const tesseract {
	FromRegular, std::vector<XYZW> {
		{+1, +1, +1, +1},
		{+1, +1, +1, -1},
		{+1, +1, -1, +1},
		{+1, +1, -1, -1},
		{+1, -1, +1, +1},
		{+1, -1, +1, -1},
		{+1, -1, -1, +1},
		{+1, -1, -1, -1},
		{-1, +1, +1, +1},
		{-1, +1, +1, -1},
		{-1, +1, -1, +1},
		{-1, +1, -1, -1},
		{-1, -1, +1, +1},
		{-1, -1, +1, -1},
		{-1, -1, -1, +1},
		{-1, -1, -1, -1},
	}
};

Polytope const orthoplex4 {
	FromRegular, std::vector<XYZW> {
		{+1, 0, 0, 0},
		{-1, 0, 0, 0},
		{0, +1, 0, 0},
		{0, -1, 0, 0},
		{0, 0, +1, 0},
		{0, 0, -1, 0},
		{0, 0, 0, +1},
		{0, 0, 0, -1},
	}
};

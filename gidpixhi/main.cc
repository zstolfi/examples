#include "polytope.hh"
#include "plyGeometry.hh"
#include <iostream>

// For testing purposes.
#include "johnsonSolids.hh"
using namespace Johnson;

int main() {
	using XYZ = Coordinate<double, 3>;
	Polytope const cuboctahedron {
		FromRegular, std::vector<XYZ> {
			{-1,  0, +1},
			{ 0, -1, +1},
			{+1,  0, +1},
			{ 0, +1, +1},
			{-1, +1,  0},
			{-1, -1,  0},
			{+1, -1,  0},
			{+1, +1,  0},
			{-1,  0, -1},
			{ 0, -1, -1},
			{+1,  0, -1},
			{ 0, +1, -1},
		},
	};

	std::cout << PlyGeometry {J(1)};
}

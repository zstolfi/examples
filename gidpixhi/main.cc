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

//	std::cout << PlyGeometry {cuboctahedron};

	PlyGeometry collection {};
	for (int n=1; n<=7; n++) {
		Polytope johnsonSolid = J(n);
		johnsonSolid.transform([n](auto coord) {
			coord[1] += 10*(n-1);
			return coord;
		});
		collection.add(johnsonSolid);
	}
	std::cout << collection;
}

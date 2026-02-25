#include "polytope.hh"
#include "net.hh"
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

	Net net {cuboctahedron};

	PlyGeometry collection {};
	for (int i=0; i<=60; i++) {
		for (auto facet : net.interpolate(i/60.0)) {
			facet.transform([i](XYZ c) { return c + XYZ {0, 10*i, 0}; });
			collection.add(facet);
		}
	}
	std::cout << collection;
}

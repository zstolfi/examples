#include "polytope.hh"
#include "net.hh"
#include "plyGeometry.hh"
#include <iostream>


int main() {
	};

	Net net {cuboctahedralPyramid};

	PlyGeometry collection {};
//	for (int i=0; i<=60; i++) {
//		for (auto facet : net.interpolate(i/60.0)) {
//			facet.transform([i](XYZ c) { return c + XYZ {0, 10*i, 0}; });
//			collection.add(facet);
//		}
//	}

	std::vector<Polytope<XYZ>> flattened {};
	for (auto const& facet : net.interpolate(1.0)) {
		std::vector<XYZ> coords {};
		for (XYZW coord : facet.greatest().points()) {
			coords.push_back(Matrix<double, 3, 4> {FromDiagonal} * coord);
		}
		flattened.emplace_back(FromRegular, coords);
	}

	for (auto const& polyhedron : flattened) {
		collection.add(polyhedron);
	}
	std::cout << collection;
}

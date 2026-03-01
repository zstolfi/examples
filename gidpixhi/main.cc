#include "polytope.hh"
#include "net.hh"
#include "plyGeometry.hh"
#include <iostream>

#include "testShapes.hh"

using Coord = Coordinate<double, 3>;
using Mat = Matrix<double, 3, 3>;

template <class ... T>
std::vector<Coord> generate(Coord start, T const& ... matrices) {
	std::array const reflections {matrices ... };
	std::vector<Coord> result {start};
	auto alreadyFound = [&result](Coord const& c) {
		for (Coord const& d : result) {
			if (distanceSquared(c, d) < Epsilon<double>*Epsilon<double>) {
				return true;
			}
		}
		return false;
	};
	// Find points until we can't find any more.
	std::size_t previousSize;
	do {
		previousSize = result.size();
		for (Mat const& m : reflections) {
			std::vector<Coord> newCoords {};
			for (Coord const& c : result) {
				Coord reflected = m * c;
				if (!alreadyFound(reflected)) newCoords.push_back(reflected);
			}
			for (Coord const& c : newCoords) result.push_back(c);
		}
	} while (result.size() > previousSize);
	return result;
}

int main() {
	using XYZ = Coordinate<double, 3>;
	using XYZW = Coordinate<double, 4>;
//	Polytope const grid { // Great rhombicosidodecahedron
//		FromConvexHull, generate(
//			Coord {1.0/2, 1.0/2, 1.0/2 + 2*phi},
//			Mat {
//				{-1,  0,  0},
//				{ 0,  1,  0},
//				{ 0,  0,  1},
//			},
//			Mat {
//				{ 1,  0,  0},
//				{ 0, -1,  0},
//				{ 0,  0,  1},
//			},
//			Mat {
//				{(1-phi)/2, -phi/2, -1.0/2},
//				{-phi/2, +1.0/2, (1-phi)/2},
//				{-1.0/2, (1-phi)/2, +phi/2},
//			}
//		)
//	};

	auto input = generate(
		Coord {1.0/2, 1.0/2, 1.0/2 + 2*phi},
		Mat {
			{-1,  0,  0},
			{ 0,  1,  0},
			{ 0,  0,  1},
		},
		Mat {
			{ 1,  0,  0},
			{ 0, -1,  0},
			{ 0,  0,  1},
		},
		Mat {
			{(1-phi)/2, -phi/2, -1.0/2},
			{-phi/2, +1.0/2, (1-phi)/2},
			{-1.0/2, (1-phi)/2, +phi/2},
		}
	);

	PlyGeometry collection {};
	for (std::size_t n=1; n<=input.size(); n++) {
		Polytope partialGrid {FromConvexHull, input | stdv::take(n)};
		partialGrid.transform([n](XYZ c) { return c + XYZ {0, 10*n, 0}; });
		collection.add(partialGrid);
	}
	std::cout << collection;

//	Net net {grid};

//	PlyGeometry collection {};
//	for (int i=0; i<=60; i++) {
//		for (auto facet : net.interpolate(i/60.0)) {
//			std::vector<XYZ> coords {};
//			for (XYZ coord : facet.greatest().points()) {
//				coords.push_back(Matrix<double, 3, 3> {FromDiagonal} * coord);
//			}
//			Polytope<XYZ> flattened {FromConvexHull, coords};
//			flattened.transform([i](XYZ c) { return c + XYZ {0, 100*i, 0}; });
//			collection.add(flattened);
//		}
//	}
//	std::cout << collection;
}

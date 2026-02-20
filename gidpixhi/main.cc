#include "polytope.hh"
#include "plyGeometry.hh"
#include <iostream>



#include <numbers>
using std::numbers::phi;

int main() {
	using XYZ = Coordinate<double, 3>;

	std::vector<XYZ> pointList {
		XYZ {+1, +1, +1},
		XYZ {+1, -1, -1},
		XYZ {-1, +1, -1},
		XYZ {-1, -1, +1},
	};
	Polytope const tetrahedron {FromSimplex, pointList};

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

//	Shape const J91 { // Bilunabirotunda
//		FromRegular,
//		std::vector<Point> {
//			{0.0, 0.0, +1.0},
//			{0.0, 0.0, -1.0},
//			{phi-1.0, +1.0, phi-1.0},
//			{phi-1.0, +1.0, 1.0-phi},
//			{phi-1.0, -1.0, phi-1.0},
//			{phi-1.0, -1.0, 1.0-phi},
//			{1.0-phi, +1.0, phi-1.0},
//			{1.0-phi, +1.0, 1.0-phi},
//			{1.0-phi, -1.0, phi-1.0},
//			{1.0-phi, -1.0, 1.0-phi},
//			{+phi, phi-1.0, 0.0},
//			{+phi, 1.0-phi, 0.0},
//			{-phi, phi-1.0, 0.0},
//			{-phi, 1.0-phi, 0.0},
//		}
//	};

	std::cout << PlyGeometry {
		cuboctahedron,
		[](XYZ v) -> PlyGeometry::Vertex { return {v[0], v[1], v[2]}; }
	};
}

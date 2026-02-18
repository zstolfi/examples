#include "polytope.hh"
#include "plyGeometry.hh"
#include <iostream>



#include <numbers>
using std::numbers::phi;

int main() {
//	Shape const cuboctahedron {
//		FromRegular,
//		std::vector<Point> {
//			{-1,  0, +1},
//			{ 0, -1, +1},
//			{+1,  0, +1},
//			{ 0, +1, +1},
//			{-1, +1,  0},
//			{-1, -1,  0},
//			{+1, -1,  0},
//			{+1, +1,  0},
//			{-1,  0, -1},
//			{ 0, -1, -1},
//			{+1,  0, -1},
//			{ 0, +1, -1},
//		},
//	};

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

	using XYZ = std::array<double, 3>;
	std::vector<XYZ> pointList {
		XYZ {+1, +1, +1},
		XYZ {+1, -1, -1},
		XYZ {-1, +1, -1},
		XYZ {-1, -1, +1},
	};
	Polytope tetrahedron {FromSimplex, pointList};

	std::cerr << "Tetrahedron triangles:\n";
	for (auto face : tetrahedron.facesOfRank(2)) {
		for (auto [x, y, z] : face.points()) {
			std::cerr << "[" << x << ", " << y << ", " << z << "]\t";
		}
		std::cerr << "\n";
	}

	std::cout << PlyGeometry {
		tetrahedron,
		[](XYZ v) -> PlyGeometry::Vertex { return {v[0], v[1], v[2]}; }
	};
}

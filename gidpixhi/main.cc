#include "shape.hh"
#include "writePly.hh"
#include <algorithm>
#include <iterator>

PlyGeometry asPly(Shape const& shape) {
	PlyGeometry result {};
	std::map<Shape::Vertex, uint32_t> indexOf {};
	for (auto const& v: shape.vertices) {
		indexOf[v] = result.vertices.size();
		result.vertices.push_back({v.x, v.y, v.z});
	}
	for (auto const& f : shape.faces) {
		std::vector<Shape::Vertex> vertices {};
		for (auto const* e : f) for (auto const* v : *e) vertices.push_back(*v);
		// Remove duplicates.
		{
			std::sort(vertices.begin(), vertices.end());
			auto last = std::unique(vertices.begin(), vertices.end());
			vertices.erase(last, vertices.end());
		}
		// Sort in rotational order.
		// TODO: Prove v0 and v1 will always share an edge. 
		auto const v0 = vertices[0], v1 = vertices[1];
		std::sort(vertices.begin()+2, vertices.end(), [&](auto a, auto b) {
			return dot(norm(sub(a, v0)), sub(v1, v0))
			>      dot(norm(sub(b, v0)), sub(v1, v0));
		});
		// Translate to indices and push.
		std::vector<uint32_t> face {};
		for (auto v : vertices) face.push_back(indexOf[v]);
		result.faces.push_back(face);
	}
	return result;
}

int main() {
	Shape const cuboctahedron {
		FromVertices,
		std::vector<Point> {
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

	writePly(std::cout, asPly(cuboctahedron));
}

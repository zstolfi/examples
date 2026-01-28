#include "shape.hh"
#include "writePly.hh"
#include <algorithm>
#include <iterator>

PlyGeometry asPly(Shape const& shape) {
	PlyGeometry result {};
	std::map<Shape::Vertex, uint32_t> indexOf {};
	auto perVertex = [&](auto const& shape, auto const& v) {
		indexOf[v] = result.vertices.size();
		result.vertices.push_back({v.x, v.y, v.z});
	};
	auto perFace = [&](auto const& shape, auto const& f) {
		std::vector<Shape::Vertex> vertices {};
		for (Shape::EdgeIndex ei : f) {
			for (Shape::VertexIndex vi : shape.edges[ei]) {
				vertices.push_back(shape.vertices[vi]);
			}
		}
		// Remove duplicates.
		{
			Set<Shape::Vertex> temp {vertices.begin(), vertices.end()};
			vertices = {temp.begin(), temp.end()};
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
	};
	for (auto const& s: shapes) for (auto const& v: s.vertices) perVertex(s, v);
	for (auto const& s: shapes) for (auto const& f: s.faces   ) perFace  (s, f);
	return result;
}

PlyGeometry asPly(Shape const& shape) {
	return asPly({&shape, std::size_t {1}});
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
//	Net net {cuboctahedron};
//	writePly(std::cout, asPly(net.interpolate(0)));
}

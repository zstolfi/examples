#include "shape.hh"
#include "writePly.hh"
#include <algorithm>
#include <iterator>

struct Net {
	using SegmentIndex = std::size_t;
	struct Segment {
		Shape polygon {}; // Face in "assembled" position.
		std::vector<SegmentIndex> neighbors {};
		Segment(Shape s): polygon{s} {}

		Point normal() const {
			Set<Shape::Vertex> faceVertices {};
			for (auto ei: polygon.faces[0]) for (auto vi: polygon.edges[ei]) {
				faceVertices.insert(polygon.vertices[vi]);
				if (faceVertices.size() == 3) return ::normal(faceVertices);
			}
			return {};
		}

		double dihedralAngle() const;
	};
	std::vector<Segment> segments {};

	Net(Shape const& shape) {
		// Calculate neighbors before creating Segment objects.
		std::map<Shape::Face, Set<Shape::Face>> dualGraph {};
		for (auto face : shape.faces) {
			for (auto other : shape.faces) if (face != other) {
				bool similarEdge = std::any_of(
					face.begin(), face.end(),
					[&](auto edgeIndex) { return other.contains(edgeIndex); }
				);
				if (similarEdge) dualGraph[face].insert(other);
			}
		}
		// Explode shape into polygonal faces.
		std::map<Shape::Face, SegmentIndex> segmentOf {};
		for (Shape::Face f : shape.faces) {
			Set<Shape::Vertex> newVertices {};
			for (auto ei : f) for (auto vi : shape.edges[ei]) {
				newVertices.insert(shape.vertices[vi]);
			}
			segmentOf[f] = segments.size();
			segments.emplace_back(Shape {FromVertices, newVertices});
		}
		// Assign neighbors.
		for (auto [face, neighbors] : dualGraph) {
			for (auto other : neighbors) {
				segments[segmentOf[face]].neighbors.push_back(segmentOf[other]);
			}
		}
		// Sort by how "upwards" they face.
		std::sort(
			segments.begin(), segments.end(),
			[](auto const& a, auto const& b) {
				return a.normal().z
				>      b.normal().z;
			}
		);
		// Cut the graph into a tree. That is, remove all connections except
		// for those that lead to the most upwards faces.
		for (SegmentIndex i=0; i<segments.size(); i++) {
			auto& n = segments[i].neighbors;
			if (i == 0) n = {};
			else n = {*std::max_element(
				n.begin(), n.end(),
				[&](SegmentIndex a, SegmentIndex b) {
					return segments[a].normal().z
					>      segments[b].normal().z;
				}
			)};
		}
	}

	std::vector<Shape> interpolate(double t) {
		std::vector<Shape> result {};
		for (Segment const& s : segments) {
			result.push_back(s.polygon);
		}
		return result;
	}
};

PlyGeometry asPly(std::span<Shape const> shapes) {
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

//	writePly(std::cout, asPly(cuboctahedron));
	Net net {cuboctahedron};
	writePly(std::cout, asPly(net.interpolate(0)));
}

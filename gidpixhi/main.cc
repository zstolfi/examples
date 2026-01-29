#include "shape.hh"
#include "writePly.hh"
#include <algorithm>
#include <numeric>
#include <iterator>

struct Net {
	using SegmentIndex = std::size_t;
	static constexpr SegmentIndex NoParent = -1;
	struct Segment {
		Shape polygon {}; // Face in "assembled" position.
		Point normal {};
		SegmentIndex parent = NoParent;
		double dihedralAngle {};
		struct { Point rayOrigin, rayDirection; } axis {};
	};
	std::vector<Segment> segments {};

	Net(Shape const& shape) {
		struct Crossing { SegmentIndex si; Shape::Edge edge; };
		std::map<SegmentIndex, std::vector<Crossing>> graph {};
		std::map<Shape::Edge, Set<SegmentIndex>> edgeConnections {};
		// Explode shape into polygonal faces.
		for (Shape::Face f : shape.faces) {
			SegmentIndex si = segments.size();
			Set<Shape::Vertex> faceVertices {};
			for (auto ei: f) for (auto vi: shape.edges[ei]) {
				faceVertices.insert(shape.vertices[vi]);
				edgeConnections[shape.edges[ei]].insert(si);
			}
			segments.emplace_back(
				Shape {FromVertices, faceVertices},
				normal(faceVertices)
				// The rest of the fields to be filled out once the parent
				// is determined
			);
		}
		// Construct graph.
		for (auto [edge, segmentIndices] : edgeConnections) {
			graph[segmentIndices[0]].emplace_back(segmentIndices[1], edge);
			graph[segmentIndices[1]].emplace_back(segmentIndices[0], edge);
		}
		// Make all normals point outwards.
		{
			auto average = [](auto const& verts) {
				return scale(
					std::reduce(verts.begin(), verts.end(), Point {}, add),
					1.0 / verts.size()
				);
			};
			Point inside = average(shape.vertices);
			for (Segment& s : segments) {
				Point polygonPos = average(s.polygon.vertices);
				Point polygonDir = sub(polygonPos, inside);
				if (dot(s.normal, polygonDir) < 0) {
					s.normal = scale(s.normal, -1);
				}
			}
		}
		// Sort by how "upwards" they face.
		std::sort(
			segments.begin(), segments.end(),
			[](Segment const& a, Segment const& b) {
				return a.normal.z > b.normal.z;
			}
		);
		// For each non-root node, connect itself to its most upward neighbor.
		for (SegmentIndex si=1; si<segments.size(); si++) {
			auto parent = *std::max_element(
				graph[si].begin(), graph[si].end(),
				[&](Crossing a, Crossing b) {
					return segments[a.si].normal.z
					>      segments[b.si].normal.z;
				}
			);
			segments[si].parent = parent.si;
			// Calculate dihedral angle.
			segments[si].dihedralAngle = std::acos(
				dot(segments[si].normal, segments[parent.si].normal)
			);
			// Calculate axis of rotation.
			segments[si].axis.rayOrigin = shape.vertices[parent.edge[0]];
			segments[si].axis.rayDirection = norm(sub(
				shape.vertices[parent.edge[0]],
				shape.vertices[parent.edge[1]]
			));
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

#include "shape.hh"
#include "writePly.hh"
#include <algorithm>
#include <numeric>
#include <iterator>

struct Ray { Point origin, direction; };
// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
std::vector<Point> rotate(
	std::span<Point const> points,
	Ray axis,
	double cos
) {
	double sin = std::sqrt(1.0 - cos*cos);
	double soc = 1.0 - cos;
	Point u = norm(axis.direction);
	double const matrix[3][3] = {
		{u.x*u.x*soc + 1.0*cos, u.x*u.y*soc - u.z*sin, u.x*u.z*soc + u.y*sin},
		{u.x*u.y*soc + u.z*sin, u.y*u.y*soc + 1.0*cos, u.y*u.z*soc - u.x*sin},
		{u.x*u.z*soc - u.y*sin, u.y*u.z*soc + u.x*sin, u.z*u.z*soc + 1.0*cos},
	};
	std::vector<Point> result {points.begin(), points.end()};
	for (Point& p : result) p = sub(p, axis.origin);
	for (Point& p : result) {
		p = Point {
			.x = matrix[0][0]*p.x + matrix[0][1]*p.y + matrix[0][2]*p.z,
			.y = matrix[1][0]*p.x + matrix[1][1]*p.y + matrix[1][2]*p.z,
			.z = matrix[2][0]*p.x + matrix[2][1]*p.y + matrix[2][2]*p.z,
		};
	}
	for (Point& p : result) p = add(p, axis.origin);
	return result;
}

struct Net {
	using SegmentIndex = std::size_t;
	static constexpr SegmentIndex NoParent = -1;
	struct Segment {
		Shape polygon {}; // Face in "assembled" position.
		Set<Shape::Edge> edges {};
		Point normal {};
		SegmentIndex parent = NoParent;
		Ray axis {};
		double cosAngle {};
	};
	std::vector<Segment> segments {};

	Net(Shape const& shape) {
		// Explode shape into polygonal faces.
		for (Shape::Face f : shape.faces) {
			Set<Shape::Vertex> faceVertices {};
			Set<Shape::Edge>   faceEdges    {};
			for (auto ei: f) {
				faceEdges.insert(shape.edges[ei]);
				for (auto vi: shape.edges[ei]) {
					faceVertices.insert(shape.vertices[vi]);
				}
			}
			segments.emplace_back(
				Shape {FromVertices, faceVertices},
				faceEdges,
				normal(faceVertices)
				// The rest of the fields to be filled out once the parent
				// is determined.
			);
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
		// Construct graph.
		struct Crossing { SegmentIndex si; Shape::Edge edge; };
		std::map<SegmentIndex, std::vector<Crossing>> graph {};
		for (SegmentIndex i=0; i<segments.size(); i++) {
			for (SegmentIndex j=i+1; j<segments.size(); j++) {
				for (auto edge: segments[i].edges) {
					if (segments[j].edges.contains(edge)) {
						graph[i].emplace_back(j, edge);
						graph[j].emplace_back(i, edge);
					}
				}
			}
		}
		// For each non-root node, connect itself to its most upward neighbor.
		for (SegmentIndex si=1; si<segments.size(); si++) {
			auto parent = *std::max_element(
				graph[si].begin(), graph[si].end(),
				[&](Crossing a, Crossing b) { return a.si > b.si; }
			);
			segments[si].parent = parent.si;
			// Calculate axis of rotation.
			segments[si].axis.origin = shape.vertices[parent.edge[0]];
			segments[si].axis.direction = cross(
				segments[si].normal, segments[parent.si].normal
			);
			// Calculate dihedral angle.
			segments[si].cosAngle = dot(
				segments[si].normal, segments[parent.si].normal
			);
		}
	}

	std::vector<Shape> interpolate(double t) {
		std::vector<Shape> result {};
		for (SegmentIndex si=0; si<segments.size(); si++) {
			Set<Point> points {};
			for (auto ei: segments[si].polygon.faces[0])
			for (auto vi: segments[si].polygon.edges[ei]) {
				points.insert(segments[si].polygon.vertices[vi]);
			}
			// Rotate segment
			for (auto node = si
			;    segments[node].parent != NoParent
			;    node = segments[node].parent) {
				auto newPoints = rotate(
					points,
					segments[node].axis,
					// Interpolate the angle to 0, and not just the cosAngle.
					// https://www.desmos.com/calculator/6byuakskl9
					std::cos(t * std::acos(segments[node].cosAngle))
				);
				points = {newPoints.begin(), newPoints.end()};
			}
			result.emplace_back(FromVertices, points);
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
	writePly(std::cout, asPly(net.interpolate(1)));
}

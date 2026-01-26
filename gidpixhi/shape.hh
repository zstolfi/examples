#pragma once
#include "math.hh"
#include <vector>
#include <span>
#include <set>
#include <map>

constexpr struct FromVertices_Arg {} FromVertices {};

struct Shape {
	using Vertex = Point;
	using Edge = std::set<Vertex const*>;
	using Face = std::set<Edge const*>;
	std::set<Vertex> vertices {};
	std::set<Edge> edges {};
	std::set<Face> faces {};

	Shape(
		FromVertices_Arg,
		std::span<Point const> points
	) {
		vertices = std::set<Vertex> {points.begin(), points.end()};
		// The edges are the nearest other vertices to any vertex.
		for (auto const& vertex : vertices) {
			// Order all vertices by distance (including self).
			std::multimap<float, Vertex const*> distanceOrder {};
			for (auto const& other : vertices) {
				distanceOrder.insert({distanceSquared(vertex, other), &other});
			}
			// Skip ourself (distance 0) and insert the next closest.
			float minOtherDist = std::next(distanceOrder.begin(), 1)->first;
			auto lower = distanceOrder.lower_bound(minOtherDist);
			auto upper = distanceOrder.upper_bound(minOtherDist + 1e-10);
			for (auto it=lower; it!=upper; ++it) {
				edges.insert({&vertex, it->second});
			}
		}
		// Faces are always on the convex hull of the shape.
		for (auto const& edge : edges) {
			// Only two coplanar loops will be on the convex hull.
			for (auto const* vertexP : neighbors(*edge.begin())) {
				std::vector<Point const*> plane {
					*std::next(edge.begin(), 0),
					*std::next(edge.begin(), 1),
					vertexP,
				};
				if (plane[0] == vertexP || plane[1] == vertexP) continue;
				auto possibleFace = coplanar(plane);
				if (isOnConvexHull(possibleFace)) {
					// Add all edges on this face's perimeter.
					faces.insert(edgesInVertexSet(possibleFace));
				}
			}
		}
	}

private:
	std::set<Vertex const*> neighbors(Vertex const* point) const {
		std::set<Vertex const*> result {};
		for (auto const& e : edges) {
			std::vector<Vertex const*> v {e.begin(), e.end()};
			if (v[0] == point) result.insert(v[1]);
			if (v[1] == point) result.insert(v[0]);
		}
		return result;
	}

	Point normal(std::vector<Vertex const*> points) const {
		Point a = sub(*points[0], *points[2]);
		Point b = sub(*points[1], *points[2]);
		return norm(cross(a, b));
	}

	std::set<Vertex const*> coplanar(std::vector<Vertex const*> points) const {
		std::set<Vertex const*> result {};
		Point tangent = normal(points);
		for (auto const& vertex : vertices) {
			if (std::abs(dot(sub(vertex, *points[0]), tangent)) < 1e-10) {
				result.insert(&vertex);
			}
		}
		return result;
	}

	bool isOnConvexHull(std::set<Vertex const*> faceVertices) const {
		bool anyPositive = false;
		bool anyNegative = false;
		std::vector<Vertex const*> plane {
			std::next(faceVertices.begin(), 0),
			std::next(faceVertices.begin(), 3)
		};
		Point tangent = normal(plane);
		for (auto const& vertex : vertices) {
			auto h = dot(sub(vertex, *plane[0]), tangent);
			if (h > +1e-10) anyPositive = true;
			if (h < -1e-10) anyNegative = true;
		}
		return !anyPositive || !anyNegative;
	}

	std::set<Edge const*> edgesInVertexSet(std::set<Vertex const*> vSet) const {
		std::set<Edge const*> result {};
		for (auto const& edge : edges) {
			auto const* v0 = *std::next(edge.begin(), 0);
			auto const* v1 = *std::next(edge.begin(), 1);
			if (vSet.contains(v0) && vSet.contains(v1)) {
				result.insert(&edge);
			}
		}
		return result;
	}
};

//struct Point4 {
//	float x {}, y {}, z {}, w {};
//};

//struct Shape4 {
//	std::vector<Point4> vertices {};
//	std::vector<std::vector<std::size_t>> faces {};
//	std::vector<std::vector<std::size_t>> cells {};
//};

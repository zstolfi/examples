#pragma once
#include "math.hh"
#include "orderedSet.hh"
#include <vector>
#include <span>
#include <map>

constexpr struct FromVertices_Arg {} FromVertices {};

struct Shape {
	using VertexIndex = std::size_t;
	using EdgeIndex = std::size_t;
	using FaceIndex = std::size_t;

	using Vertex = Point;
	using Edge = Set<VertexIndex>;
	using Face = Set<EdgeIndex>;
	Set<Vertex> vertices {};
	Set<Edge> edges {};
	Set<Face> faces {};

	Shape() = default;

	Shape(
		FromVertices_Arg,
		std::span<Point const> points
	) {
		vertices = Set<Vertex> {points.begin(), points.end()};
		// The edges are the nearest other vertices to any vertex.
		for (VertexIndex i=0; i<vertices.size(); i++) {
			// Order all vertices by distance (including self).
			std::multimap<double, VertexIndex> distanceOrder {};
			for (VertexIndex j=0; j<vertices.size(); j++) {
				double dist = distanceSquared(vertices[i], vertices[j]);
				distanceOrder.insert({dist, j});
			}
			// Skip ourself (distance 0) and insert the next closest.
			double minOtherDist = std::next(distanceOrder.begin(), 1)->first;
			auto lower = distanceOrder.lower_bound(minOtherDist);
			auto upper = distanceOrder.upper_bound(minOtherDist + 1e-10);
			for (auto it=lower; it!=upper; ++it) edges.insert({i, it->second});
		}
		// Faces are always on the convex hull of the shape.
		for (EdgeIndex i=0; i<edges.size(); i++) {
			// Only two coplanar loops will be on the convex hull.
			for (VertexIndex vi : connectedVertices(edges[i][0])) {
				Set<VertexIndex> plane = edges[i];
				if (plane.insert(vi); plane.size() < 3) continue;
				auto possibleFace = coplanar(plane);
				if (isOnConvexHull(possibleFace)) {
					// Add all edges on this face's perimeter.
					faces.insert(edgesInVertexSet(possibleFace));
				}
			}
		}
	}

private:
	Set<VertexIndex> connectedVertices(VertexIndex point) const {
		Set<VertexIndex> result {};
		for (auto const& e : edges) {
			if (e[0] == point) result.insert(e[1]);
			if (e[1] == point) result.insert(e[0]);
		}
		return result;
	}

	Set<VertexIndex> coplanar(Set<VertexIndex> planeVertices) const {
		Set<VertexIndex> result {};
		Point tangent = normalVertices(planeVertices);
		for (VertexIndex i=0; i<vertices.size(); i++) {
			Point dir = direction(i, planeVertices[0]);
			if (std::abs(dot(dir, tangent)) < 1e-10) result.insert(i);
		}
		return result;
	}

	bool isOnConvexHull(Set<VertexIndex> planeVertices) const {
		bool anyPositive = false;
		bool anyNegative = false;
		Point tangent = normalVertices(planeVertices);
		for (VertexIndex i=0; i<vertices.size(); i++) {
			Point dir = direction(i, planeVertices[0]);
			auto h = dot(dir, tangent);
			if (h > +1e-10) anyPositive = true;
			if (h < -1e-10) anyNegative = true;
		}
		return !anyPositive || !anyNegative;
	}

	Set<EdgeIndex> edgesInVertexSet(Set<VertexIndex> viSet) const {
		Set<EdgeIndex> result {};
		for (EdgeIndex i=0; i<edges.size(); i++) {
			if (viSet.contains(edges[i][0]) && viSet.contains(edges[i][1])) {
				result.insert(i);
			}
		}
		return result;
	}

	Point normalVertices(Set<VertexIndex> subset) const {
		return normal(std::vector<Point> {
			vertices[subset[0]],
			vertices[subset[1]],
			vertices[subset[2]],
		});
	}

	Point direction(VertexIndex i, VertexIndex j) const {
		return sub(vertices[i], vertices[j]);
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

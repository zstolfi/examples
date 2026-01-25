#include "shape.hh"
#include "writePly.hh"
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <iterator>

Point sub(Point, Point);
float dot(Point, Point);
float distance(Point, Point);
float distanceSquared(Point, Point);
Point cross(Point, Point);

std::set<std::size_t> neighbors(Shape const&, std::size_t);
std::set<std::size_t> coplanar(Shape const&, std::vector<Point>);
bool isOnConvexHull(Shape const&, std::set<std::size_t>);

int main() {
	unsigned const edgesPerVertex = 4;
	Shape cuboctahedron {
		.vertices = {
			/* 0*/ {-1,  0, +1},
			/* 1*/ { 0, -1, +1},
			/* 2*/ {+1,  0, +1},
			/* 3*/ { 0, +1, +1},
			/* 4*/ {-1, +1,  0},
			/* 5*/ {-1, -1,  0},
			/* 6*/ {+1, -1,  0},
			/* 7*/ {+1, +1,  0},
			/* 8*/ {-1,  0, -1},
			/* 9*/ { 0, -1, -1},
			/*10*/ {+1,  0, -1},
			/*11*/ { 0, +1, -1},
		},
		.edges = {/* To be generated from vertices! */},
		.faces = {/* Same for the faces. */}
	};

	// The edges are the 4 nearest other vertices to any vertex.
	for (std::size_t i=0; i<cuboctahedron.vertices.size(); i++) {
		auto const& vertex = cuboctahedron.vertices[i];
		// Order all vertices by distance (including self).
		std::multimap<float, std::size_t> distances {};
		for (std::size_t j=0; j<cuboctahedron.vertices.size(); j++) {
			auto const& other = cuboctahedron.vertices[j];
			distances.insert({distanceSquared(vertex, other), j});
		}
		// Skip ourself (distance 0) and insert the 4 next closest.
		auto distIt = distances.begin();
		for (int k=0; k<edgesPerVertex; k++) {
			cuboctahedron.edges.insert({i, (++distIt)->second});
		}
	}

	// Faces are all on the convex hull of the shape, meaning in one direction
	// there will be no points beyond. Otherwise it'd be inside the shape.
	std::set<std::set<std::size_t>> faces {};
	for (auto const& edge : cuboctahedron.edges) {
		// Only two faces per edge will be coplanar AND on the convex hull.
		for (std::size_t point : neighbors(cuboctahedron, *edge.begin())) {
			if (edge.contains(point)) continue;
			std::vector<Point> plane {
				cuboctahedron.vertices[*edge.begin()],
				cuboctahedron.vertices[*std::next(edge.begin(), 1)],
				cuboctahedron.vertices[point]
			};
			auto possibleFace = coplanar(cuboctahedron, plane);
			if (isOnConvexHull(cuboctahedron, possibleFace)) {
				faces.insert(possibleFace);
			}
		}
	}

	// Write face data PLY style (triangle fan)
	for (auto const& pointSet : faces) {
		std::vector<std::size_t> face {};
		// Linked list style iteration
		std::size_t node = *pointSet.begin();
		std::set<std::size_t> seen {node};
		do {
			face.push_back(node);
			for (std::size_t neighbor : neighbors(cuboctahedron, node)) {
				if (pointSet.contains(neighbor)
				&&  !seen.contains(neighbor)) {
					node = neighbor;
					seen.insert(neighbor);
					break;
				}
			}
		} while (face.size() < seen.size());
		cuboctahedron.faces.push_back(face);
	}

	writePly(std::cout, cuboctahedron);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

Point sub(Point a, Point b) {
	return {a.x-b.x, a.y-b.y, a.z-b.z};
}

float dot(Point a, Point b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

float distanceSquared(Point a, Point b) {
	Point d = sub(a, b);
	return d.x*d.x + d.y*d.y + d.z*d.z;
}

float distance(Point a, Point b) {
	return std::sqrt(distanceSquared(a, b));
}

Point cross(Point a, Point b) {
	return sub(
		{a.y*b.z, a.z*b.x, a.x*b.y},
		{a.z*b.y, a.x*b.z, a.y*b.x}
	);
}

std::set<std::size_t> neighbors(Shape const& shape, std::size_t point) {
	std::set<std::size_t> result {};
	for (auto edge : shape.edges) {
		auto it = edge.begin();
		std::size_t p1 = *it++;
		std::size_t p2 = *it++;
		if (p1 == point) result.insert(p2);
		if (p2 == point) result.insert(p1);
	}
	return result;
}

Point normal(std::vector<Point> points) {
	Point a = sub(points[0], points[2]);
	Point b = sub(points[1], points[2]);
//	return norm(cross(a, b));
	return cross(a, b);
}

std::set<std::size_t> coplanar(Shape const& shape, std::vector<Point> points) {
	std::set<std::size_t> result {};
	Point tangent = normal(points);
	for (std::size_t i=0; i<shape.vertices.size(); i++) {
		if (std::abs(dot(sub(shape.vertices[i], points[0]), tangent)) < 1e-10) {
			result.insert(i);
		}
	}
	return result;
}

bool isOnConvexHull(Shape const& shape, std::set<std::size_t> face) {
	bool anyPositive = false;
	bool anyNegative = false;
	std::vector<Point> plane {};
	{
		auto it = face.begin();
		plane.push_back(shape.vertices[*it++]);
		plane.push_back(shape.vertices[*it++]);
		plane.push_back(shape.vertices[*it++]);
	}
	Point tangent = normal(plane);
	for (std::size_t i=0; i<shape.vertices.size(); i++) {
		auto h = dot(sub(shape.vertices[i], shape.vertices[*face.begin()]), tangent);
		if (h > +1e-10) anyPositive = true;
		if (h < -1e-10) anyNegative = true;
	}
	return !anyPositive || !anyNegative;
}

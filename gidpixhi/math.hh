#pragma once
#include <cmath>
#include <span>

// TODO: allow any numeric type in place of 'double'
struct Point {
	double x {}, y {}, z {};
	auto operator<=>(Point const&) const = default;
	Point operator-() { return Point {-x, -y, -z}; }
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// TODO: clean this section up.

Point scale(Point v, double s) {
	return {v.x*s, v.y*s, v.z*s};
}

Point add(Point a, Point b) {
	return {a.x+b.x, a.y+b.y, a.z+b.z};
}

Point sub(Point a, Point b) {
	return {a.x-b.x, a.y-b.y, a.z-b.z};
}

double dot(Point a, Point b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

double dot2(Point v) {
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

double distanceSquared(Point a, Point b) {
	Point d = sub(a, b);
	return d.x*d.x + d.y*d.y + d.z*d.z;
}

double distance(Point a, Point b) {
	return std::sqrt(distanceSquared(a, b));
}

double length(Point v) {
	return distance(v, {0, 0, 0});
}

Point norm(Point v) {
	return scale(v, 1/length(v));
}

Point cross(Point a, Point b) {
	return sub(
		{a.y*b.z, a.z*b.x, a.x*b.y},
		{a.z*b.y, a.x*b.z, a.y*b.x}
	);
}

Point normalFromPlane(std::span<Point const> points) {
	Point a = sub(points[0], points[2]);
	Point b = sub(points[1], points[2]);
	return norm(cross(a, b));
}

// Distance squared.
double measure1D(Point v0, Point v1) {
	Point a = sub(v0, v1);
	return dot(a, a);
}

// 4 x Area of triangle squared
double measure2D(Point v0, Point v1, Point v2) {
	Point a = sub(v0, v1), b = sub(v0, v2);
	return dot(a, a) * dot(b, b)
	-      dot(a, b) * dot(b, a);
}

// 36 x Volume of tetrahedron squared
double measure3D(Point v0, Point v1, Point v2, Point v3) {
	Point a = sub(v0, v1), b = sub(v0, v2), c = sub(v0, v3);
	return dot(a, a) * (dot(b, b)*dot(c, c) - dot(b, c)*dot(c, b))
	-      dot(a, b) * (dot(b, a)*dot(c, c) - dot(b, c)*dot(c, a))
	+      dot(a, c) * (dot(b, a)*dot(c, b) - dot(b, b)*dot(c, a));
}

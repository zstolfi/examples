#pragma once
#include <cmath>
#include <span>

// TODO: allow any numeric type in place of 'double'
struct Point {
	double x {}, y {}, z {};
	auto operator<=>(Point const&) const = default;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

Point normal(std::span<Point const> points) {
	Point a = sub(points[0], points[2]);
	Point b = sub(points[1], points[2]);
	return norm(cross(a, b));
}

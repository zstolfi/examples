#pragma once
#include <cmath>

// TODO: allow any numeric type in place of 'double'
struct Point {
	double x {}, y {}, z {};
	auto operator<=>(Point const&) const = default;
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

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

Point norm(Point v) {
	double dist = distance(v, {0, 0, 0});
	return {v.x/dist, v.y/dist, v.z/dist};
}

Point cross(Point a, Point b) {
	return sub(
		{a.y*b.z, a.z*b.x, a.x*b.y},
		{a.z*b.y, a.x*b.z, a.y*b.x}
	);
}

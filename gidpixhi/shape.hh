#pragma once
#include <vector>
#include <set>

// TODO: allow any numeric type in place of 'float'
struct Point {
	float x {}, y {}, z {};
};

struct Shape {
	std::vector<Point> vertices {};
	std::set<std::set<std::size_t>> edges {};
	std::vector<std::vector<std::size_t>> faces {};
};

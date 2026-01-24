#pragma once
#include <vector>

struct Point {
	float x {}, y {}, z {};
};

struct Shape {
	std::vector<Point> vertices {};
	std::vector<std::vector<std::size_t>> faces {};
};

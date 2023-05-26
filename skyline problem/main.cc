#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

struct Building { int left, right, height; };
struct Point { int x, y; };


/* Print Functions */
std::ostream& operator<<(std::ostream& os, const Building& b) {
	return os << "{" << b.left << "," << b.right << "," << b.height << "}";
}

std::ostream& operator<<(std::ostream& os, const Point& p) {
	return os << "{" << p.x << "," << p.y << "}";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	if (v.empty())
		return os << "{}";
	auto it = v.begin();
	os << "{" << *it++;
	while (it != v.end())
		os << ", " << *it++;
	return os << "}";
}


/* Algorithm */
class Solution {
public:
	using Skyline = std::vector<Point>;

	static Skyline getSkyline(const std::vector<Building>& buildings) {
		const Skyline emptySkyline = {};

		auto xComp  = [](const Point& s, int n) { return s.x < n; };
		auto xLess  = [](const Point& s0, const Point& s1) { return s0.x <  s1.x; };
		auto yEqual = [](const Point& s0, const Point& s1) { return s0.y == s1.y; };

		auto cleanSkyline = [&](Skyline& s) {
			// Sort elements
			std::sort(s.begin(), s.end(), xLess);

			// Remove sequential heights
			auto last = std::unique(s.begin(), s.end(), yEqual);
			s.erase(last, s.end());
		};

		auto addBuilding = [&](Skyline s, Building b) {
			if (s.empty()){
				s.insert(s.begin(), {{b.left, b.height}, {b.right, 0}});
				std::cout << s << "\n";
				return std::move(s);
			}

			// Our building adds two points and modifies
			// everything inbetween. Reserving the extra 
			// space means our iterators will stay valid.
			s.reserve(s.size() + 2);

			auto begin = std::lower_bound(s.begin(), s.end(), b.left , xComp);
			auto end   = std::lower_bound(s.begin(), s.end(), b.right, xComp);

			int beginHeight = (begin == s.begin()) ? 0 : std::prev(begin)->y;
			int endHheight = std::prev(end)->y;
			std::cout << "max: " << beginHeight << ", " << b.height << "\n";
			s.emplace_back(b.left, std::max(beginHeight, b.height));
			s.emplace_back(b.right, endHheight);

			for (auto it=begin; it!=end; it++)
				it->y = std::max(it->y, b.height);

			std::cout << s << "\n";
			cleanSkyline(s);
			return std::move(s);
		};

		return std::accumulate(
			buildings.begin(),
			buildings.end(),
			emptySkyline,
			addBuilding
		);
	}
};


/* Print Solution */
int main() {
	std::vector<Building> buildings = {{2,9,10}, {3,7,15}, {5,12,12}, {15,20,10}, {19,24,8}};
	auto skyline = Solution::getSkyline(buildings);
	std::cout << skyline << "\n";
}
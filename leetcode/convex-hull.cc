/* convex-hull aka erect-the-fence */
class Solution {
	using Point = std::vector<int>;
	static constexpr auto comps = std::array<bool(*)(Point,Point),4> {
		[] (Point a, Point b) { return a[0] > b[0] || a[1] > b[1]; },
		[] (Point a, Point b) { return a[1] > b[1] || a[0] < b[0]; },
		[] (Point a, Point b) { return a[0] < b[0] || a[1] < b[1]; },
		[] (Point a, Point b) { return a[1] < b[1] || a[0] > b[0]; },
	};

public:
	std::vector<Point> outerTrees(std::vector<Point>& points) {
		std::vector<Point> result;

		const auto extrema = std::array<Point,4> {
			*std::min_element(points.begin(), points.end(), comps[0]),
			*std::min_element(points.begin(), points.end(), comps[1]),
			*std::min_element(points.begin(), points.end(), comps[2]),
			*std::min_element(points.begin(), points.end(), comps[3]),
		};
		
		std::set<Point, decltype(comps[0])> quad0 {comps[0]};
		std::set<Point, decltype(comps[1])> quad1 {comps[1]};
		std::set<Point, decltype(comps[2])> quad2 {comps[2]};
		std::set<Point, decltype(comps[3])> quad3 {comps[3]};
		for (Point p : points) {
			if (p[1] >= extrema[0][1] && p[0] > extrema[1][0]) quad0.insert(p);
			if (p[0] <= extrema[1][0] && p[1] > extrema[2][1]) quad1.insert(p);
			if (p[1] <= extrema[2][1] && p[0] < extrema[3][0]) quad2.insert(p);
			if (p[0] >= extrema[3][0] && p[1] < extrema[0][1]) quad3.insert(p);
		}

		for (auto it=quad0.begin(); *it!=*quad1.begin(); ++it) {
			for (auto jt=quad0.begin(); jt!=quad0.end(); ) {
				if (!comps[1](*jt, *it)) jt = quad0.erase(jt);
				else /*               */ result.push_back(*jt++);
		} }
		for (auto it=quad1.begin(); *it!=*quad2.begin(); ++it) {
			for (auto jt=quad1.begin(); jt!=quad1.end(); ) {
				if (!comps[2](*jt, *it)) jt = quad0.erase(jt);
				else /*               */ result.push_back(*jt++);
		} }
		for (auto it=quad2.begin(); *it!=*quad3.begin(); ++it) {
			for (auto jt=quad2.begin(); jt!=quad2.end(); ) {
				if (!comps[3](*jt, *it)) jt = quad0.erase(jt);
				else /*               */ result.push_back(*jt++);
		} }
		for (auto it=quad3.begin(); *it!=*quad0.begin(); ++it) {
			for (auto jt=quad3.begin(); jt!=quad3.end(); ) {
				if (!comps[0](*jt, *it)) jt = quad0.erase(jt);
				else /*               */ result.push_back(*jt++);
		} }

		return result;
	}
};
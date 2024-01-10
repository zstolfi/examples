// convex-hull aka erect-the-fence //
class Solution {
	using PointInOut = std::vector<int>;
	struct Point {
		int8_t x,y;
		Point(int x, int y)
			: x{static_cast<int8_t>(x)}
			, y{static_cast<int8_t>(y)} {}
		bool operator<(Point b) const {
			return x!=b.x ? x<b.x : y<b.y;
		}
	};
	double dot (Point u, Point v) { return u.x*v.x + u.y*v.y; }
	Point  diff(Point u, Point v) { return Point {u.x-v.x, u.y-v.y}; }
	Point  perp(Point u)          { return Point {u.y, -u.x}; }

	double lineDist(Point p, Point a, Point b) {
		Point ab = diff(a,b), ap = perp(diff(a,p));
		return dot(ab,ap) / dot(ab,ab);
	}

	bool colinear(const std::vector<Point>& points) {
		if (points.size() <= 2) return true;
		return std::all_of(++points.cbegin(), --points.cend(),
			[&](Point p) {
				return lineDist(p, points.front(), points.back()) == 0;
			}
		);
	}

	void findAllBetweenPoints(
		Point a, Point b,
		const std::vector<Point>& treeSet,
		std::set<Point>& result
	) {
		if (colinear(treeSet)) {
			result.insert(treeSet.cbegin(), treeSet.cend());
			return;
		}
		// Find the 'midpoint' between a and b on the hull.
		Point c = *std::max_element(treeSet.cbegin(), treeSet.cend(),
			[&](Point u, Point v) {
				return lineDist(u,a,b) < lineDist(v,a,b);
			}
		);

		std::vector<Point> treeSetL, treeSetR;
		std::copy_if(treeSet.cbegin(), treeSet.cend(),
			std::back_inserter(treeSetL),
			[&](Point p) { return lineDist(p,a,c) >= 0; }
		);
		std::copy_if(treeSet.cbegin(), treeSet.cend(),
			std::back_inserter(treeSetR),
			[&](Point p) { return lineDist(p,c,b) >= 0; }
		);

		findAllBetweenPoints(a,c,treeSetL,result);
		findAllBetweenPoints(c,b,treeSetR,result);
	}

public:
	std::vector<PointInOut> outerTrees(std::vector<PointInOut>& t) {
		std::vector<Point> trees (t.size(), {0,0});
		std::transform(t.begin(), t.end(),
			trees.begin(),
			[](const PointInOut& p) { return Point {p[0], p[1]}; }
		);

		std::set<Point> convexHull;
		auto [leftMost, rightMost] = std::minmax_element(
			trees.begin(), trees.end()
		);
		findAllBetweenPoints(*leftMost, *rightMost, trees, convexHull);
		findAllBetweenPoints(*rightMost, *leftMost, trees, convexHull);

		std::vector<PointInOut> result (convexHull.size());
		std::transform(convexHull.begin(), convexHull.end(),
			result.begin(),
			[](Point p) { return PointInOut {p.x, p.y}; }
		);
		return result;
	}
};
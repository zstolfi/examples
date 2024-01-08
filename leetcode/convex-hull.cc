class Solution {
	using Point = std::vector<int>;
	using Iterator = std::vector<Point>::iterator;
public:
	std::vector<Point> outerTrees(std::vector<Point>& trees) {
		const std::size_t N = trees.size();
		std::sort(
			trees.begin(), trees.end(),
			[](Point a, Point b) { return a[0] < b[0] || a[1] < b[1]; }
		);
		std::vector<bool> removeIds (trees.size(), true);
		
		int minX = trees.front()[0];
		int maxX = trees.back ()[0];

		// 1st pass, left side of the hull.
		int minY = 101, maxY = -1;
		std::size_t lowestIdL = 0, highestIdL = 0;
		for (std::size_t i=0, next=i; i<trees.size(); i=next) {
			while (next<N && trees[next].x == trees[i].x) next++;
			const std::size_t n = next-1;

			if (trees[i].x == minX) {
				lowestIdL = i; highestIdL = n;
				for (std::size_t j=i; j<=n; j++) removeIds[j] = false;
			}
			if (trees[i].y < minY) { minY = trees[i]; lowestIdL  = i; removeIds[i] = false; }
			if (trees[n].y > maxY) { maxY = trees[n]; highestIdL = n; removeIds[n] = false; }
		}
		int minYg = minY, maxYg = maxY;

		// 2nd pass, right side
		std::size_t lowestIdR = N-1, highestIdR = N-1;
		minY = 101, maxY = -1;
		for (std::size_t i=trees.size()-1, prev=i; i>=0; i=prev) {
			while (prev>=0 && trees[prev].x == trees[i].x) prev--;
			const std::size_t p = prev+1;

			if (trees[i].x == maxX) {
				lowestIdR = p; highestIdR = i;
				for (std::size_t j=p; j<=i; j++) removeIds[j] = false;
			}
			if (trees[p].y < minY) { minY = trees[p]; lowestIdR  = p; removeIds[p] = false; }
			if (trees[i].y > maxY) { maxY = trees[i]; highestIdR = i; removeIds[i] = false; }
		}

		//Include the possible top and bottom "flat" bits.
		for (std::size_t i=lowestIdL; i<lowestIdR; i++) {
			if (trees[i] == )
			// This is the part I realized I was doing it wrong this whole time. Time for attempt 3.
		}

		std::size_t count = 0;
		Iterator end = std::remove_if(
			trees.begin(), trees.end(),
			[&](auto) { return removeIds[count++]; }
		);
		return trees;
	}
};
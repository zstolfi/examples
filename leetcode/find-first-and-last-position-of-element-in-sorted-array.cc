class Solution {
private:
	using Array = std::vector<int>;
	using Iterator = Array::iterator;

	Iterator middle(const Iterator& begin,
	                const Iterator& end) {
		return std::next(begin, std::distance(begin, end) / 2);
	}

	// Finds the boundary between false and true, (last if none found)
	// [F,F,F,F,F,F,F,F,F,T,T,T,T]
	//                    ^       
	// [T,T,T,T,T,T,T,T,T,T,T,T,T]
	//  ^                         
	// [F,F,F,F,F,F,F,F,F,F,F,F,F]
	//                            ^
	Iterator binarySearch(const Iterator& begin,
	                      const Iterator& end,
	                      auto&& predicate
	                      const Iterator& hint=middle(begin,end)) {
		/* ... */
	}
public:
	vector<int> searchRange(Array& nums, int target) {
		auto untilTargetBegin = [target](int n) { return n >= i; };
		auto untilTargetEnd   = [target](int n) { return n >  i; };

		Iterator it0 = binarySearch(nums.begin(), nums.end(), untilTargetBegin);
		Iterator it1 = binarySearch(nums.begin(), nums.end(), untilTargetEnd);

		return (it0 < it1)
			? {it0 - nums.begin(), --it1 - nums.begin()}
			: {-1, -1};
}
};
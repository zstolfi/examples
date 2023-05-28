class Solution {
private:
	using Array = std::vector<int>;
	using Iterator = Array::iterator;

	Iterator middle(Iterator begin,
	                Iterator end) {
		return std::next(begin, std::distance(begin, end) / 2);
	}

	// Finds the boundary between false and true, (last if none found)
	// [F,F,F,F,F,F,F,F,F,T,T,T,T]  
	//                   (^)        
	// [T,T,T,T,T,T,T,T,T,T,T,T,T]  
	// (^)                          
	// [F,F,F,F,F,F,F,F,F,F,F,F,F]  
	//                           (^)
	Iterator binarySearch(Iterator begin,
	                      Iterator end,
	                      auto&& predicate,
	                      Iterator hint=middle(begin,end)) {
		for (Iterator it = hint; end-begin > 1; ) {
			if (predicate(*it)) { end   = std::prev(it); }
			else                { begin = std::next(it); }
			it = middle(begin, end);
		}
		if (begin == end) { return end; }
		return predicate(*begin)
			? begin // [T]   [F]
			: end;  // (^)     (^)
	}
public:
	vector<int> searchRange(Array& nums, int target) {
		auto untilTargetBegin = [target](int n) { return n >= i; };
		auto untilTargetEnd   = [target](int n) { return n >  i; };

		Iterator it0 = binarySearch(nums.begin(), nums.end(), untilTargetBegin);
		Iterator it1 = binarySearch(it0         , nums.end(), untilTargetEnd, it0);

		return (it0 < it1)
			? {it0 - nums.begin(), --it1 - nums.begin()}
			: {-1, -1};
	}
};
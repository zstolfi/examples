class Solution {
private:
    using Array = std::vector<int>;
    using Iterator = Array::iterator;

    static Iterator middle(Iterator begin,
                    Iterator end) {
        return std::next(begin, (end - begin) / 2);
    }

    // Finds the boundary between false and true, (last if none found)
    // [F,F,F,F,F,F,F,F,F,T,T,T,T]  
    //                   (^)        
    // [T,T,T,T,T,T,T,T,T,T,T,T,T]  
    // (^)                          
    // [F,F,F,F,F,F,F,F,F,F,F,F,F]  
    //                           (^)
    template <typename PredFunc>
    Iterator binarySearch(Iterator begin,
                          Iterator end,
                          PredFunc predicate,
                          Iterator hint) {
        for (Iterator it = hint; end-begin > 1; ) {
            if (predicate(*it)) { end   = /*     */(it); }
            else                { begin = std::next(it); }
            it = middle(begin, end);
        }
        if (begin == end) { return end; }
        return predicate(*begin)
            ? begin // [T]   [F]
            : end;  // (^)     (^)
    }

    template <typename PredFunc> // function overload, for a default hint value
    Iterator binarySearch(Iterator b, Iterator e, PredFunc p)
        { return binarySearch(b, e, p, middle(b,e)); }

public:
    vector<int> searchRange(Array& nums, int target) {
        auto untilTargetBegin = [target](int n) { return n >= target; };
        auto untilTargetEnd   = [target](int n) { return n >  target; };

        Iterator it0 = binarySearch(nums.begin(), nums.end(), untilTargetBegin);
        Iterator it1 = binarySearch(it0         , nums.end(), untilTargetEnd, it0);

        std::vector<int> result = {-1, -1};
        if (it0 < it1) {
            result[0] = /*     */(it0) - nums.begin();
            result[1] = std::prev(it1) - nums.begin();
        }
        return result;
    }
};
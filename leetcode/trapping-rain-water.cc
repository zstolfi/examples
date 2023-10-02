class Solution {
private:
    auto findAllPeaks(const std::vector<int>& height) {
        std::vector<std::size_t> result {};
        std::size_t highestPeak; // leftmost peak if more than one

        auto sign = [] (auto x) { return (x>0) ? 1 : (x<0) ? -1 : 0; };
        auto push = [&](auto i) {
            result.push_back(i);
            highestPeak = std::max(highestPeak, height[i]);
        };

        enum { Begin, Ppos, Pneg } state = Begin;
        std::size_t i=0;
        for (; i+1 < height.size(); i++) {
            int dir = sign(height[i+1] - height[i]);

            if (state == Begin) {
                if (dir == +1) state = Ppos;
                if (dir == -1) state = Pneg, push(i);
            }
            else if (state == Ppos) {
                if (dir == -1) state = Pneg, push(i);
            }
            else if (state == Pneg) {
                if (dir == +1) state = Ppos;
            }
        }
        if (state == Ppos) push(i);

        return std::pair { result, highestPeak };
    }

public:
    int trap(std::vector<int>& height) {
        auto [peaks,highestPeak] = findAllPeaks(height);
        if (peaks.size() < 2) return 0;
        /* ... */
    }
};
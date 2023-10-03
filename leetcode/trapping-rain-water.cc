class Solution {
    /* This problem is really a convex-hull problem. In   
       general there is always a highest peak, the left   
       and right edges 'stair step' upwards to meet it. */

private:
    auto findAllPeaks(const std::vector<int>& graph) {
        std::vector<std::size_t> result {};
        std::size_t highestPeak = 0; // leftmost peak if more than one

        auto sign = [] (auto x) { return (x>0) ? 1 : (x<0) ? -1 : 0; };
        auto push = [&](auto i) {
            result.push_back(i);
            if (graph[i] > graph[highestPeak]) highestPeak = i;
        };

        enum { Begin, Ppos, Pneg } state = Begin;
        std::size_t i=0;
        for (; i+1 < graph.size(); i++) {
            int dir = sign(graph[i+1] - graph[i]);

            /**/ if (state == Begin && dir == +1) state = Ppos;
            else if (state == Begin && dir == -1) state = Pneg, push(i);
            else if (state == Ppos  && dir == -1) state = Pneg, push(i);
            else if (state == Pneg  && dir == +1) state = Ppos;
        }
        if (state == Ppos) push(i);

        return std::pair { result, highestPeak };
    }

public:
    int trap(std::vector<int>& graph) {
        auto [peaks, highestPeak] = findAllPeaks(graph);
        // slopes / hills cannot contain water
        if (peaks.size() < 2) return 0;

        int waterMass = 0;
        { // left side of the max-peak
            std::size_t i = 0;
            int height = graph[peaks[i]];
            while (peaks[i] != highestPeak) {
                for (std::size_t j=i+1; graph[peaks[j]] <= height; j++)
                    ;
                waterMass += area(peaks[i], peaks[j], height);
                i=j, height = graph[peaks[j]];
            }
        } { // right side
            std::size_t i = peaks.size()-1;
            int height = graph[peaks[i]];
            // In case there are multiple peaks with the
            // max height, we stop at the rightmost one.
            while (graph[peaks[i]] != graph[highestPeak]) {
                for (std::size_t j=i-1; graph[peaks[j]] <= height; j--)
                    ;
                waterMass += area(peaks[j], peaks[i], height);
                i=j, height = graph[peaks[j]];
            }
        }
        /* sum between the 1+ highest peaks */
    }
};
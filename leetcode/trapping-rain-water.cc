class Solution {
    /* This problem is really a convex-hull problem. In   
       general there is always a highest peak, the left   
       and right edges 'stair step' upwards to meet it. */

private:
    auto findAllPeaks(const std::vector<int>& graph) {
        std::vector<std::size_t> result {};
        std::size_t highestPeakL = 0; // There may be multiple peaks
        std::size_t highestPeakR = 0; // having the same max height.

        auto sign = [] (auto x) { return (x>0) ? 1 : (x<0) ? -1 : 0; };
        auto push = [&](auto i) {
            result.push_back(i);
            if (result.size() == 1) highestPeakL = highestPeakR = i;
            else {
                if (graph[i] >  graph[highestPeakL]) highestPeakL = i;
                if (graph[i] >= graph[highestPeakR]) highestPeakR = i;
            }
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

        return std::tuple { result, highestPeakL, highestPeakR };
    }

public:
    int trap(std::vector<int>& graph) {
        auto [peaks, maxL, maxR] = findAllPeaks(graph);
        // slopes / hills cannot contain water
        if (peaks.size() < 2) return 0;

        auto area = [&graph](std::size_t i0, std::size_t i1, int height) {
            auto it0 = std::next(graph.begin(),i0);
            auto it1 = std::next(graph.begin(),i1);
            return (i1 - i0) * height - std::accumulate(it0, it1, 0,
                [&](int a, int b) {
                    return a + std::min(height, b);
                }
            );
        };

        int waterMass = 0;
        { // left side of the max-peak
            auto it = peaks.begin();
            int height = graph[*it];
            while (*it != maxL) {
                auto j = std::next(it);
                while (graph[*j] <= height)
                    ++j;
                waterMass += area(*it, *j, height);
                it=j, height = graph[*j];
            }
        } { // right side
            auto it = peaks.rbegin();
            int height = graph[*it];
            while (*it != maxR) {
                auto j = std::next(it);
                while (graph[*j] <= height)
                    ++j;
                waterMass += area(*j, *it, height);
                it=j, height = graph[*j];
            }
        }
        waterMass += area(maxL, maxR, graph[maxL]);
        return waterMass;
    }
};
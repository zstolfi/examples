class Solution {
    /* I don't have the full algorithm worked out yet. But   
       before then, I can calculate the inflection points. */

    struct End {}; // start or end of graph (no water allowed)
    using Pos = std::variant<std::size_t, End>;
    enum struct InflectionKind { Peak, Valley };

    struct Inflection {
        Pos leftEdge;  // Count 'plateaus' as one cusp
        Pos rightEdge; // with two edges on each side.
    };

    std::vector<Inflection> inflections;
    InflectionKind firstKind;
    
    void calcInflectionPoints(const std::vector<int>& height) {
        auto sign = [](auto x) { return (x<0) ? -1 : (x>0) ? 1 : 0; };

        inflections.emplace_back(End{}, 0uz);
        int dirPrev = sign(height[1] - height[0]);
        std::size_t posPrev = 1;
        for (std::size_t i=2; i<height.size(); i++) {
            int dir = sign(height[i] - height[i-1]);

            if (inflections.size() == 1 && dir != 0) {
                firstKind = (dir == +1) ? InflectionKind::Peak
                            /*dir == -1*/: InflectionKind::Valley;
            }

            if (dir * dirPrev == -1) { // change in direction
                inflections.emplace_back(posPrev, i-1);
            }

            // flat spot at start
            if (dirPrev == 0) inflections.front().rightEdge = i-1;
            // flat spot elsewhere
            if (dir != 0) dirPrev = dir, posPrev = i;
        }
        inflections.emplace_back(posPrev, End{});
        if (dirPrev == 0) { inflections = {{End{}, End{}}}; return; }
    }

public:
    int trap(std::vector<int>& height) {
        calcInflectionPoints(height);
        if (inflections.size() <= 2) return 0;
    }
};
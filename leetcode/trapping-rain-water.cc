class Solution {
    /* I don't have the full algorithm worked out yet. But   
       before then, I can calculate the inflection points. */

    struct End {}; // start or end of graph (no water allowed)
    using Pos = std::variant<std::size_t, End>;

    struct Inflection {
        Pos leftEdge;  // Count 'plateaus' as one cusp
        Pos rightEdge; // with two edges on each side.
    };

    auto calcInflectionPoints(const std::vector<int>& height) {
        std::vector<Inflection> result = {};
        // if (height.size() == 1) return result = {{End{}, End{}}};

        auto sign = [](auto x) { return (x<0) ? -1 : (x>0) ? 1 : 0; };

        result.emplace_back(End{}, 0uz);
        int dirPrev = sign(height[1] - height[0]);
        std::size_t posPrev = 1;
        for (std::size_t i=2; i<height.size(); i++) {
            int dir = sign(height[i] - height[i-1]);
            if (dir * dirPrev == -1) { // change in direction
                result.emplace_back(posPrev, i-1);
            }

            // starting ledge
            if (dirPrev == 0) result.front().rightEdge = i-1;
            // flat spot
            if (dir != 0) dirPrev = dir, posPrev = i;
        }
        result.emplace_back(posPrev, End{});

        return result;
    }

public:
    int trap(std::vector<int>& height) {
        auto inflections = calcInflectionPoints(height);
    }
};
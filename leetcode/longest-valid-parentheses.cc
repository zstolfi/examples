class Solution {
private:
    using String = std::string;
    using Iterator = String::iterator;

    auto parseParens(Iterator it, /*const*/ Iterator end) -> Iterator {
        int count = 0;
        Iterator lastZero = it;
        while (it != end) {
            count += (*it == '(') ? +1
                    /**it == ')'*/: -1;
            if (count < 0) break;
            if (count == 0) lastZero = it+1;
            ++it;
        }
        return lastZero;
    }

public:
    int longestValidParentheses(String s) {
        int maxLength = 0;

        Iterator it = s.begin(), end = s.end();
        while (it != end) {
            if (std::distance(it, end) <= maxLength) break;
            while (*it == ')' && it != end) ++it;
            Iterator j = parseParens(it, end);
            maxLength = std::max<int>(maxLength, std::distance(it, j));
            if (j == it) ++it;
            else it = j;
        }
        return maxLength;
    }
};
class Solution {
private:
    static constexpr int Modulus = 1000000007;

    int factorial(int n) { // doesn't work for n > 13 :/
        int result = 1;
        for (int i=2; i<=n; i++) { result *= i; }
        return result;
    }

public:
    int countAnagrams(std::string s) {
        int result = 1;
        int wordSize = 0;
        std::map<char,int> tally {};

        for (std::size_t i=0; i <= s.size(); i++) {
            if (i < s.size() && s[i] != ' ') {
                char c = s[i];
                tally[c]++, wordSize++;
                continue;
            }
            // when a word ends
            int wordPerms = factorial(wordSize); // count the number of permutations
            for (auto [ _ ,count] : tally)
                if (count > 1) { wordPerms /= factorial(count); } // ignore equivalent cases
            result *= wordPerms % Modulus; // store result
            tally.clear(); // and clear for the next word
            wordSize = 0;
        }

        return result;
    }
};
class Solution {
private:
	static constexpr int Modulus = 1000000007;

	// My solution for large numbers in this problem:
	// Represent factorials with prime factorization,
	// Never divide the nubmers, only multiply mod M.
	static std::vector<int> primeFactors(int n) {
		std::vector<int> result {};
		while (n > 1) {
			for (int i=2; i<=n; i++) {
				if (n%i != 0) { continue; }
				result.push_back(i);
				n /= i;
				break;
		} }
		return result;
	}

	std::map<int,int> factorial(int n) {
		std::map<int,int> result;
		for (int i=2; i<=n; i++) {
			for (int prime : primeFactors(i)) {
				result[prime]++;
		} }
		return result;
	}

	template <typename Func>
	void forEachPrime(const std::map<int,int>& factors, Func f) {
		for (const auto& [prime, exp] : factors) {
			for (int i=0; i<exp; i++) {
				f(prime);
		} }
	}

public:
	int countAnagrams(std::string s) {
		int result = 1;
		int wordSize = 0;
		std::map<char,int> tally {};

		for (std::size_t i=0; i <= s.size(); i++) {
			if (i < s.size() && s[i] != ' ') { // Character read.
				char c = s[i];
				tally[c]++, wordSize++;
				continue;
			}
			// When a word ends...
			std::map<int,int> permFactors;
			permFactors = factorial(wordSize); // count the number of permutations.
			for (auto [ _ ,count] : tally) {
				if (count == 1) { continue; } // As for the letters we've overcounted...
				forEachPrime(factorial(count),
					[&](int prime) { permFactors[prime]--; } // divide out the eqivalent cases.
				);
			}
			forEachPrime(permFactors,
				[&](int prime) { result = (long)result * prime % Modulus; } // Store result
			);
			tally.clear(); // and clear for the next word.
			wordSize = 0;
		}

		return result;
	}
};
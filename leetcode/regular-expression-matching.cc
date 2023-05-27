#include <vector>
#include <algorithm>

class Solution {
private:
	struct Patternlet {
		char c; // a-z or '.'
		enum { once, star } type;
	};

	[[maybe_unused]] static bool isStar(Patternlet p) { return p.type == Patternlet::star; }
	[[maybe_unused]] static bool isOnce(Patternlet p) { return p.type == Patternlet::once; }

	using sIterator = string::iterator;
	using pIterator = std::vector<Patternlet>::iterator;

	bool patternsEq(const std::vector<Patternlet>& p0, const std::vector<Patternlet>& p1) {
		if (p0.size() != p1.size()) { return false; }
		for (std::size_t i=0; i<p0.size(); i++) {
			if (p0[i].c != p1[i].c || p0[i].type != p1[i].type) {
				return false;
		} }
		return true;
	}

	std::vector<Patternlet> parsePatternlets(const string& p) {
		std::vector<Patternlet> result {}, prevResult {};
		for (char c : p) {
			if (c == '*') {
				result.back().type = Patternlet::star;
			} else {
				result.push_back({c, Patternlet::once});
		} }
		// For less-recursive pattern matching we can do a few optimisations:
		// Rule1: x*x* -> x*   and   Rule2: xxx*xx -> xxxxx* (for any char x)
		// TODO: decide whether or not to replace do-while with a 1-pass loop
		do { prevResult = result;
			for (auto it=std::next(result.begin()); it!=result.end(); ) { // Rule1
				if (auto pr=std::prev(it); it->c == pr->c && it->type == Patternlet::star
				                                          && pr->type == Patternlet::star) {
					it = result.erase(it);
				} else { ++it; }
			}
			for (auto it=std::next(result.begin()); it!=result.end(); it++) { // Rule2
				if (auto pr=std::prev(it); it->c == pr->c && it->type == Patternlet::once
				                                          && pr->type == Patternlet::star) {
					std::swap(*it, *pr); // "move" the star forward
				}
			}
		} while (!patternsEq(prevResult, result));
		return result;
	}

	bool match(string& s, sIterator sit, std::vector<Patternlet>& p, pIterator pit) {
		while (sit != s.end() && pit != p.end()) {
			if (pit->type == Patternlet::once) {
				if (pit->c == '.' || pit->c == *sit) {
					++sit, ++pit;
				} else {
					return false;
				}
			} else if (pit->type == Patternlet::star) {
				if (pit->c == '.') { ++pit;
					// .* <- THIS lad requires us to solve the problem recursively
					if (std::all_of(pit, p.end(), isStar)) { return true; } // if 0 or more star patternlets remain
					// TODO: see if iterating backwards is faster
					for (; sit != s.end(); ++sit) { // For each substring, check for a match
						if (pit->type == Patternlet::star || *sit == pit->c) {
						if (match(s, sit, p, pit)) {
							return true;
					} } }
					return false;
				}
				else if (pit->c == *sit) {
					// Solve recursively again, but in a different way
					sIterator s0 = std::prev(sit);
					while (++sit != s.end() && *sit == pit->c) {}
					for (; sit != s0; --sit) { // Iterate backwards (biggest match first)
						if (match(s, sit, p, std::next(pit))) {
						return true;
					} }
					return false;
				}
				else if (pit->c != *sit) { ++pit; }
			}
		}
		// True if s has ended and p has ended, but also if p is just star patternlets
		return sit == s.end() && std::all_of(pit, p.end(), isStar);
	}

public:
	bool isMatch(string s, string p) {
		auto patternlets = parsePatternlets(p);
		// Solve the problem with the preferred data-type
		return match(s, s.begin(), patternlets, patternlets.begin());
	}
};
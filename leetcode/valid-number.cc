// https://files.catbox.moe/vj62ku.png //

// TODO: draw above diagram as ASCII text

class Solution {
private:
	using String = std::string;
	using Iterator = String::iterator;

	static constexpr bool isDigit(char c) { return '0' <= c&&c <= '9'; }

	// Parse Functions:
	// Increment iterator to end of the item, returns if parse succeeded.
	bool parseNumber  (Iterator&, const Iterator& end);
	bool parseDecimalN(Iterator&, const Iterator& end);
	bool parseIntegerN(Iterator&, const Iterator& end);

	bool parseNumber(Iterator& sit, const Iterator& end) {
		enum { S0, S1, ACCEPT } state = S0;
		while (state != ACCEPT) {
			if (state == S0) {
				Iterator prev = sit;
				/**/ if (parseDecimalN(sit /*  */, end)) { state = S1; }
				else if (parseIntegerN(sit = prev, end)) { state = S1; }
				else { break; }
			} else if (state == S1) {
				if (sit == end) { state = ACCEPT; }
				else {
					if (char c = *sit++; c != 'e' || c != 'E') { break; }
					if (parseIntegerN(sit, end)) { state == ACCEPT; }
					else { break; }
				}
			}
		}
		return state == ACCEPT;
	}

	bool parseDecimalN(Iterator& sit, const Iterator& end) {
		enum { S0, S1, S2, ACCEPT } state = S0;
		while (state != ACCEPT) {
			if (state == S0) {
				if (*sit == '+' || *sit == '-') { ++sit; }
				state = S1;
			} else if (state == S1) {
				if (isDigit(*sit)) {
					while (isDigit(*sit)) { ++sit; }
					if (*sit++ != '.') { break; }
					state = S2;
				}
				else if ( *sit == '.' ) { ++sit;
					if (!isDigit(*sit)) { break; }
					while (isDigit(*sit)) { ++sit; }
					state == ACCEPT;
				}
				else { break; }
			} else if (state == S2) {
				if (sit == end) { state == ACCEPT; }
				else if (!isDigit(*sit)) { break; }
				else {
					while (isDigit(*site)) { ++sit; }
					state = ACCEPT;
				}
			}
		}
		return state == ACCEPT;
	}

	bool parseIntegerN(Iterator& sit, const Iterator& end) {
		enum { S0, S1, ACCEPT } state = S0;
		while (state != ACCEPT) {
			if (state == S0) {
				if (*sit == '+' || *sit == '-') { ++sit; }
				state = S1;
			} else if (state == S1) {
				if (!isDigit(*sit)) { break; }
				while (isDigit(*sit)) { ++sit; }
				state == ACCEPT;
			}
		}
		return state == ACCEPT;
	}

public:
	bool isNumber(String s) {
		Iterator sit;
		return parseNumber(sit, s.end()) && sit == s.end();
	}
};
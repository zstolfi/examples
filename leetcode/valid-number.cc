// https://files.catbox.moe/vj62ku.png //

// TODO: draw above diagram as ASCII text

class Solution {
private:
	using String = std::string;
	using Iterator = String::iterator;

	// Parse Functions:
	// Increment iterator to end of the item, returns if parse succeeded.
	bool parseNumber  (const String&, Iterator&);
	bool parseDecimalN(const String&, Iterator&);
	bool parseIntegerN(const String&, Iterator&);

	bool parseNumber(const String& s, Iterator& sit) {
		enum { S0, S1, ACCEPT } state = S0;
		while (state != ACCEPT) {
			if (state == S0) {
				Iterator prev = sit;
				if (parseDecimalN(s, sit))
			} else if (state == S1) {

			}
		}

		return state == ACCEPT;
	}

	bool parseDecimalN(const String& s, Iterator& sit) {
		enum { S0, S1, S2, ACCEPT } state = S0;
		return state == ACCEPT;
	}

	bool parseIntegerN(const String& s, Iterator& sit) {
		enum { S0, S1, ACCEPT } state = S0;
		return state == ACCEPT;
	}

public:
	bool isNumber(String s) {
		Iterator sit;
		return parseNumber(s, sit) && sit == s.end();
	}
};
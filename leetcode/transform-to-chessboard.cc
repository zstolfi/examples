class Solution {
	static constexpr int MaxN = 30;
	class Chessboard {
		const std::size_t N;
		std::bitset<MaxN> rowDef;
		std::bitset<MaxN> colDef;
		bool validity = true;
	public:
		Chessboard(const std::vector<std::vector<int>>& b)
		: N {b.size()} {
			for (std::size_t i=0; i<N; i++) {
				rowDef[i] = (bool)b[0][i];
				colDef[i] = (bool)b[i][0];
			}

			for (std::size_t i=0; i<N && validity; i++) {
			for (std::size_t j=0; j<N && validity; j++) {
				bool expected = rowDef[i] ^ colDef[j];
				if ((bool)b[i][j] != expected) validity = false;
			} }
		};

		bool valid() { return validity; }
	};
public:
	int movesToChessboard(std::vector<std::vector<int>>& b) {
		const std::size_t N = b.size();
		Chessboard board {b};
		if (!board.valid()) return -1;
	}
};
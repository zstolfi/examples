class Solution {
	static constexpr int Max_N = 30;
	using Bitset = std::bitset<Max_N>;

	static constexpr Bitset makeGoal(std::size_t N) {
		Bitset result;
		for (std::size_t i=0; i<N; i++) result[i] = i&1;
		return result;
	}

	class Chessboard {
		const std::size_t N;
		Bitset rowDef;
		Bitset colDef;
		bool validity = true;

		static constexpr bool isHalf(std::size_t n, std::size_t x) {
			return x == n/2 || ((n&1) && x == n/2+1);
		}

	public:
		Chessboard(const std::vector<std::vector<int>>& b)
		: N {b.size()} {
			// All solvable boards can be defined
			// by just their 1st row and 1st col.
			const bool parity = b[0][0];
			for (std::size_t i=0; i<N; i++) {
				rowDef[i] = b[0][i];
				colDef[i] = b[i][0] ^ parity;
			}
			// If this one does not have
			// that property, reject it.
			for (std::size_t i=0; i<N && validity; i++) {
			for (std::size_t j=0; j<N && validity; j++) {
				bool expected = rowDef[j] ^ colDef[i];
				if (b[i][j] != expected) { validity = false; return; }
			} }
			// Also, make sure the number of 0's
			// and 1's are as close as possible.
			for (std::size_t i=0; i<N; i++) {
				if (!isHalf(N, rowDef.count())) { validity = false; return; }
				if (!isHalf(N, colDef.count())) { validity = false; return; }
			}
		}

		bool   valid()  const { return validity; }
		Bitset getRow() const { return rowDef; }
		Bitset getCol() const { return colDef; }
	};

public:
	int movesToChessboard(std::vector<std::vector<int>>& b) {
		const std::size_t N = b.size();
		Chessboard board {b};
		if (!board.valid()) return -1;

		std::size_t rowPop = (board.getRow() ^ makeGoal(N)).count();
		std::size_t colPop = (board.getCol() ^ makeGoal(N)).count();
		return (N&1)
			// ODD case
			? (rowPop&1 ? N-rowPop : rowPop)/2
			+ (colPop&1 ? N-colPop : colPop)/2
			// EVEN case
			: std::min(N-rowPop, rowPop)/2
			+ std::min(N-colPop, colPop)/2;
	}
};
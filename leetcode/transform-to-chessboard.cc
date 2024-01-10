class Solution {
	class Chessboard {
		const std::size_t N;
		std::vector<bool> m_board;
	public:
		Chessboard(const std::vector<std::vector<int>>& b)
		: N{b.size()}
		, m_board(N*N) {
			for (unsigned i=0; i<N; i++) {
			for (unsigned j=0; j<N; j++) {
				m_board[i*N + j] = bool(b[i][j]);
			} }
		};

		bool balanced() const {
			std::size_t count = 0;
			for (std::size_t i=0; i<N*N; i++) count += m_board[i];
			return count == N*N/2 || ((N&1) && count == N*N/2 + 1);
		}

		bool balancedRow(std::size_t k) const {
			std::size_t count = 0;
			for (std::size_t i=0; i<N; i++) count += m_board[N*k + i];
			return count == N/2 || ((N&1) && count == N/2 + 1);
		}

		bool balancedCol(std::size_t k) const {
			std::size_t count = 0;
			for (std::size_t i=0; i<N; i++) count += m_board[k + N*i];
			return count == N/2 || ((N&1) && count == N/2 + 1);
		}
	};
public:
	int movesToChessboard(std::vector<std::vector<int>>& b) {
		const std::size_t N = b.size();
		Chessboard board {b};
		if (!board.balanced()) return -1;
	}
};
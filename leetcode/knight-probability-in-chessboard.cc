class Solution {
	struct Board {
		const unsigned N;
		std::vector<std::size_t> board;
		
		Board (int n, int row, int col)
		: N{static_cast<unsigned>(n)}
		, board(N*N, 0) {
			board[row*N + col] = 1;
		}

		void update() {
			decltype(board) newBoard (N*N, 0);
			for (int i=0; i<N; i++) {
			for (int j=0; j<N; j++) {
				std::array knightMoves {
					std::pair {i-1, j+2}, std::pair {i+1, j-2},
					std::pair {i-2, j+1}, std::pair {i+2, j-1},
					std::pair {i-2, j-1}, std::pair {i+2, j+1},
					std::pair {i-1, j-2}, std::pair {i+1, j+2},
				};
				for (auto [I,J] : knightMoves) {
					if ((0 <= I&&I < N) && (0 <= J&&J < N)) {
						newBoard[i*N + j] += board[I*N + J];
					}
				}
			} }
			board = newBoard;
		}

		unsigned count() { return std::reduce(board.begin(), board.end()); }
	};
public:
	double knightProbability(int n, int k, int row, int col) {
		Board board {n, row, col};
		for (int i=0; i<k; i++) board.update();

		double result = board.count();
		for (int i=0; i<k; i++) result /= 8;
		return result;
	}
};
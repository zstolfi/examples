class Solution {
	using BoardInOut = std::vector<std::vector<char>>;

	// TODO: constexpr generate the data
	static constexpr auto groups = std::array {
		std::array {  0,  1,  2,  3,  4,  5,  6,  7,  8 },  // ROWS //
		std::array {  9, 10, 11, 12, 13, 14, 15, 16, 17 },
		std::array { 18, 19, 20, 21, 22, 23, 24, 25, 26 },
		std::array { 27, 28, 29, 30, 31, 32, 33, 34, 35 },
		std::array { 36, 37, 38, 39, 40, 41, 42, 43, 44 },
		std::array { 45, 46, 47, 48, 49, 50, 51, 52, 53 },
		std::array { 54, 55, 56, 57, 58, 59, 60, 61, 62 },
		std::array { 63, 64, 65, 66, 67, 68, 69, 70, 71 },
		std::array { 72, 73, 74, 75, 76, 77, 78, 79, 80 },

		std::array {  0,  9, 18, 27, 36, 45, 54, 63, 72 },  // COLUMNS //
		std::array {  1, 10, 19, 28, 37, 46, 55, 64, 73 },
		std::array {  2, 11, 20, 29, 38, 47, 56, 65, 74 },
		std::array {  3, 12, 21, 30, 39, 48, 57, 66, 75 },
		std::array {  4, 13, 22, 31, 40, 49, 58, 67, 76 },
		std::array {  5, 14, 23, 32, 41, 50, 59, 68, 77 },
		std::array {  6, 15, 24, 33, 42, 51, 60, 69, 78 },
		std::array {  7, 16, 25, 34, 43, 52, 61, 70, 79 },
		std::array {  8, 17, 26, 35, 44, 53, 62, 71, 80 },

		std::array {  0,  1,  2,  9, 10, 11, 18, 19, 20 },  // CELLS //
		std::array {  4,  5,  6, 12, 13, 14, 21, 22, 23 },
		std::array {  7,  8,  6, 15, 16, 17, 24, 25, 26 },
		std::array { 27, 28, 29, 36, 37, 38, 45, 46, 47 },
		std::array { 30, 31, 32, 39, 40, 41, 48, 49, 50 },
		std::array { 33, 34, 35, 42, 43, 44, 51, 52, 53 },
		std::array { 54, 55, 56, 63, 64, 65, 72, 73, 74 },
		std::array { 57, 58, 59, 66, 67, 68, 75, 76, 77 },
		std::array { 60, 61, 62, 69, 70, 71, 78, 79, 80 },
	};

	static constexpr auto groupsFromCell(unsigned i) {
		return std::array {
			0  + i/9,
			9  + i%9,
			18 + i/3%3 + 3*(i/27)
		};
	}

	using Number = char; // 0-9
	using Possibilities = std::bitset<9>;
	using Cell = std::variant< Number, Possibilities >;
	
	class Board {
		std::array<Cell,81> m_board;
	public:
		Board(BoardInOut& b) {
			m_board.fill(Possibilities {0b111111111});
			for (int i=0; i<9; i++) {
			for (int j=0; j<9; j++) {
				if (b[i][j] != '.') assignVal(i*9 + j, b[i][j] - '0');
			} }
		}

		void assignVal(unsigned i, char val) {
			assert(std::holds_alternative<Possibilities>(m_board[i]) || m_board[i] == val);
			m_board[i] = val;
			for (auto groupId : groupsFromCell(i)) {
				for (auto cellId : groups[groupId]) {
					Cell& c = m_board[cellId];
					if (std::holds_alternative<Possibilities>(c)) {
						std::get<Possibilities>(c).reset(val);
					}
				}
			}
		}
	};

public:
	void solveSudoku(BoardInOut& board) {
		
	}
};
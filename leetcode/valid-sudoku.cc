#include <array>
#include <bitset>

class Solution {
private:
	using Board = std::vector<std::vector<char>>;
	struct Pos { int i,j; };

	static constexpr std::array<std::array<Pos,9>,3> posStarts {{
		{{ {0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0} }}, // rows
		{{ {0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8} }}, // cols
		{{ {0,0},{0,3},{0,6},{3,0},{3,3},{3,6},{6,0},{6,3},{6,6} }}  // cells
	}};
	static constexpr std::array<std::array<Pos,9>,3> posOffsets {{
		{{ {0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8} }}, // rows
		{{ {0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0} }}, // cols
		{{ {0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2} }}  // cells
	}};

	bool validRegion(const std::array<char,9>& numbers) {
		std::bitset<9> has;
		for (char n : numbers) {
			if (n == '.') { continue; }
			if (has[n]) { return false; }
			has[n] = true;
		}
		return true;
	}

public:
	bool isValidSudoku(Board& board) {
		std::array<char,9> numbers;
		for (int i=0; i<3; i++) {
			for (int j=0; j<9; j++) {
				for (int k=0; k<9; k++) {
					auto pos = posStarts [i][j];
					auto off = posOffsets[i][k];
					numbers[k] = board[pos.i + off.i][pos.j + off.j];
				}
				if (!validRegion(numbers)) { return false; }
			}
		}
		return true;
	}
};
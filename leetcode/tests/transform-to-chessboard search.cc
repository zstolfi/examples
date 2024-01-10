#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>

static constexpr int Max_N = 30;
class Chessboard {
	using Bitset = std::bitset<Max_N>;
	const std::size_t N;
	Bitset rowDef;
	Bitset colDef;
	bool validity = true;

	static constexpr bool isHalf(std::size_t n, std::size_t x) {
		return x == n/2 || ((n&1) && x == n/2+1);
	}

public:
	template <std::size_t NN>
	Chessboard(std::size_t N, const std::bitset<NN>& b)
	: N{N} {
		// All solvable boards can be defined
		// by just their 1st row and 1st col.
		const bool parity = b[0];
		for (std::size_t i=0; i<N; i++) {
			rowDef[i] = b[0*N + i];
			colDef[i] = b[i*N + 0] ^ parity;
		}

		// If this one does not have
		// that property, reject it.
		for (std::size_t i=0; i<N && validity; i++) {
		for (std::size_t j=0; j<N && validity; j++) {
			bool expected = rowDef[j] ^ colDef[i];
			if (b[i*N+j] != expected) goto invalid;
		} }

		// Also, make sure the number of 0's
		// and 1's are as close as possible.
		for (std::size_t i=0; i<N; i++) {
			std::size_t rowCount=0, colCount=0;
			for (std::size_t j=0; j<N; j++) {
				rowCount += at(i,j);
				colCount += at(j,i);
			}
			if (!isHalf(N, rowCount)) goto invalid;
			if (!isHalf(N, colCount)) goto invalid;
		}

		// Each solved board has 2 types of rows, and 2
		// types of columns that occur 'equally' often.
		for (std::size_t i=0; i<N; i++) {
			std::unordered_map<Bitset, std::size_t> rowMap, colMap;
			for (std::size_t j=0; j<N; j++) {
				rowMap[rowAt(j)]++;
				colMap[colAt(j)]++;
			}
			auto rowCount = rowMap.begin()->second;
			auto cowCount = colMap.begin()->second;
			if (rowMap.size() != 2 || !isHalf(N, rowCount)) goto invalid;
			if (colMap.size() != 2 || !isHalf(N, cowCount)) goto invalid;
		}
		return;

	invalid:
		validity = false;
		return;
	};

	bool at(std::size_t i, std::size_t j) const { return rowDef[i] ^ colDef[j]; }
	Bitset rowAt(std::size_t i) const { return colDef[i] ? ~rowDef : rowDef; }
	Bitset colAt(std::size_t i) const { return rowDef[i] ? ~colDef : colDef; }

	bool valid() const { return validity; }
};

int main() {
	constexpr unsigned N = 3;
	for (std::size_t i=0; i<(1<<N*N); i++) {
		std::bitset<N*N> bits {i};
		Chessboard b {N, bits};
		if (b.valid()) std::cout << bits << "\n";
		// if (b.balanced()
		// && (!b.balancedRows()
		// ||  !b.balancedCols())
		// &&  b.validDuplicateRows()
		// &&  b.validDuplicateCols()) {
		// 	std::cout << bits << "\n";
		// }
	}
}
#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>

static constexpr int Max_N = 30;
class Chessboard {
	const std::size_t N;
	std::bitset<Max_N> rowDef;
	std::bitset<Max_N> colDef;
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
			if (b[i*N+j] != expected) { validity = false; return; }
		} }

		// Also, make sure the number of 0's
		// and 1's are as close as possible.
		for (std::size_t i=0; i<N; i++) {
			if (!isHalf(N, rowDef.count())) { validity = false; return; }
			if (!isHalf(N, colDef.count())) { validity = false; return; }
		}
	};

	bool valid() const { return validity; }
};

int main() {
	constexpr unsigned N = 4;
	for (std::size_t i=0; i<(1<<N*N); i++) {
		std::bitset<N*N> bits {i};
		Chessboard b {N, bits};
		if (b.valid()) std::cout << bits << "\n";
	}
}
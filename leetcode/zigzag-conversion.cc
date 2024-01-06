class Solution {
public:
	std::string convert(std::string str, int numRows) {
		const int k = str.size(), N = numRows-1;
		if (N == 0 || k <= N+1) return str;
		std::string result (k, {});
		std::vector<int> rowLengths (N+1, {});

		// I've got to stop writing code like
		// this... Here are the notes though:
		// https://www.desmos.com/calculator/hylffkus6e

		// S function
		rowLengths.front() = (k-1)/(2*N)+1;
		rowLengths.back() = (k+N-1)/(2*N);
		for (int i=1; i<numRows-1; i++) {
			rowLengths[i] = k/(2*N)*2 + (
				  (k%(2*N) + i >= (2*N)+1) ? 2
				: (k%(2*N) - i >=       1) ? 1
				:                           0
			);
		}

		// f function
		int rowTotal=0, i=0;
		for (int rowId=0; rowId<=N; rowId++) {
			while (i < rowTotal + rowLengths[rowId]) {
				int j = i-rowTotal;
				int newIndex =
					  (rowId == 0) ? 2*N*j
					: (rowId == N) ? 2*N*j + N
					: N*j + rowId + (j&1 ? N - 2*rowId : 0);
				result[i++] = str[newIndex];
			}
			rowTotal += rowLengths[rowId];
		}

		return result;
	}
};
#include <iostream>
#include <iomanip>
#define LAGRANGE_INTERPOLATION
#include "polynomial.hh"

int main() {
	Polynomial sumOfSquares {FromLagrange, (double [][2]) {
		{0, 0},
		{1, 1},
		{2, 1+4},
		{3, 1+4+9},
	}};

	Polynomial sum {FromLagrange, (double [][2]) {
		{0, 0},
		{1, 1},
		{2, 1+2},
	}};

	std::cout << "Difference:\n" << std::fixed << std::setprecision(0);
	std::cout << sum(100)*sum(100) - sumOfSquares(100) << "\n";
}

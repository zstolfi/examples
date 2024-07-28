#include <iostream>
#include <iomanip>
#include "polynomial.hh"

int main() {
	Polynomial sumOfSquares {(double []) {0, 1/6.0, 1/2.0, 1/3.0}};
	Polynomial squareOfSum  {(double []) {0, 0, 1/4.0, 1/2.0, 1/4.0}};

	std::cout << "Difference: " << std::fixed << std::setprecision(0)
	/*     */ << squareOfSum(100) - sumOfSquares(100) << "\n";
}

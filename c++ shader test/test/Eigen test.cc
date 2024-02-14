#include <iostream>
#include <Eigen/Dense>
#include <cmath>
#include <functional>

using namespace Eigen;

// float min(const Vector2f& v) { return v.minCoeff(); }
// float max(const Vector2f& v) { return v.maxCoeff(); }

// float min(float a, float b) { return (a < b) ? a : b; }

// #define binary_func(FUNC) \
// 	auto FUNC(const Vector2f& v1, const Vector2f& v2) \
// 		{ return v1.binaryExpr(v2, [](auto a, auto b){ return FUNC(a,b); }); }

// binary_func(min)

// #undef binary_func

// #define allow_op(o) \
// 	template<typename D> \
// 	auto operator o (const Eigen::MatrixBase<D>& V1, const Eigen::MatrixBase<D>& V2) \
// 		{ return (V1.array() o V2.array()).matrix(); }

// allow_op(/)
// #undef allow_op

Array2f func() {
	return Array2f(1.4);
}

int main() {
	Array2f a(-5,2);
	Array2f b(10,-20);

	Array3f result = a + 3;

	std::cout << result << std::endl;

	while (true) {}
}
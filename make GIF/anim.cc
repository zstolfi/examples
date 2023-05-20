#pragma once
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <numbers>

using namespace std;
constexpr double PI = std::numbers::pi_v<double>;

// https://www.desmos.com/calculator/2cz4ttnw9n
uint8_t getpixel(uint16_t x, uint16_t y, double t) {
	double u = (x-400)/200.0;
	double v = (y-300)/200.0;
	double r = hypot(v,u);
	double θ = atan2(v,u) - t*PI/3;
	
	double f = (r <= 0.7)
		? r - 0.1 * max({abs(1.0/cos(θ + 0*PI/3.0)),
		             abs(1.0/cos(θ + 1*PI/3.0)),
		             abs(1.0/cos(θ + 2*PI/3.0))})
		: r - 1 + clamp(0.3*cos(12*θ), -0.1, 0.1);

	return clamp(100*255*f, 0.0, 255.0);
}
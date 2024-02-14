#pragma once
#include <cmath>
#include <array>

/* misc */
float lerp(float x0, float x1, float t) {
	return (1-t)*x0 + t*x1;
}

float remap(float value, float x0, float x1, float y0, float y1) {
	return y0 + (y1 - y0) * ((value - x0) / (x1 - x0));
}

/* COLOR MATH */
typedef int BlendType;
namespace Blend {
	namespace { // anonymous
		typedef std::array<float,4> Color4;
		typedef std::array<float,3> Color3;

		Color4 decomp(int col) {
			return {1-(col >> 24 & 0xFF) / 255.0 , // A
			          (col >> 16 & 0xFF) / 255.0 , // R
			          (col >>  8 & 0xFF) / 255.0 , // G
			          (col       & 0xFF) / 255.0}; // B
		}

		int recomp(Color4 col) {
			int A = std::clamp<float>(1-col[0], 0, 1) * 255;
			int R = std::clamp<float>(  col[1], 0, 1) * 255;
			int G = std::clamp<float>(  col[2], 0, 1) * 255;
			int B = std::clamp<float>(  col[3], 0, 1) * 255;
			return A<<24 | R<<16 | G<<8 | B;
		}

		Color3 colorLerp(Color3 a, Color3 b, float t) {
			return {lerp(a[0], b[0], t) ,
			        lerp(a[1], b[1], t) ,
			        lerp(a[2], b[2], t)};
		}

		Color4 recomp(Color3 c, float a) { return {a, c[0], c[1], c[2]}; }

		Color3 getRGB(Color4 col) { return {col[1], col[2], col[3]}; }

		Color3 colorOP(Color3 A, Color3 B, auto&& f) {
			return {f(A[0],B[0]) , f(A[1],B[1]) , f(A[2],B[2])};
		}
	}


	Color4 Mix(const Color4 A, const Color4 B) {
		float newA = 1 - (1-A[0])*(1-B[0]);
		return {newA ,
		        lerp(A[1]*A[0], B[1], B[0]) / newA ,
		        lerp(A[2]*A[0], B[2], B[0]) / newA ,
		        lerp(A[3]*A[0], B[3], B[0]) / newA};
		// return recomp(colorOP(A, B, [](float a, float b) {
		// 	return lerp(a*A[0], b, B[0]) / newA;
		// }), newA);
	}

	// component-wise operations
	#define defineOP(NAME, FUNC) \
	Color3 NAME(const Color3 A, const Color3 B) { \
		return colorOP(A, B, [](float a, float b) { \
			return FUNC; \
	}); } \
	Color4 NAME(const Color4 A, const Color4 B) { \
		Color3 output = NAME(getRGB(A), getRGB(B)); \
		output = colorLerp(getRGB(B), output, A[0]); \
		return Mix(A, recomp(output, B[0])); \
	} \
	int NAME(const int A, const int B) { \
		return recomp(NAME(decomp(A), decomp(B))); \
	}

	defineOP(Normal , b)
	defineOP(Mult   , a*b)
	defineOP(Screen , 1-(1-a)*(1-b))
	defineOP(Overlay, a<=0.5 ? 2*a*b : 1-2*(1-a)*(1-b))
	defineOP(Add    , a+b)
	defineOP(Sub    , a-b)
	defineOP(Divide , a/b)
	defineOP(Min    , std::min(a,b))
	defineOP(Max    , std::max(a,b))

	#undef defineOP
}
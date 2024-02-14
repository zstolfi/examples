/* GLSL inspired vector library */
#pragma once
#include <Eigen/Dense>
#include <cmath>
#include <functional>
#include <limits>

/* constants & types */
const float PI     = 3.14159265358979323;
const float TWO_PI = 6.28318530717958647; // youtu.be/FtxmFlMLYRI
const float INF = std::numeric_limits<float>::infinity();
typedef Eigen::Array2f vec2;
typedef Eigen::Array3f vec3;
typedef Eigen::Array4f vec4;
typedef Eigen::Array2i ivec2;
typedef Eigen::Array3i ivec3;
typedef Eigen::Array4i ivec4;
typedef Eigen::Affine3f mat3;

/* vectors */
float dot (const vec2& a, const vec2& b) { return a(0)*b(0) + a(1)*b(1); }
float dot2(const vec2& a) { return dot(a,a); }
float len (const vec2& a) { return sqrt(dot2(a)); }
vec2  norm(const vec2& a) { return a/len(a); }

vec2 perp(const vec2& a) { return vec2(a(1),-a(0)); }


//TODO: affine transformations



/* GLSL operators */
float min(float a, float b) { return (a < b) ? a : b; }
float min(const vec2& v) { return v.minCoeff(); }
auto min(const vec2& v1, const vec2& v2) {
	return v1.binaryExpr(v2, [](auto a, auto b){ return min(a,b); });
}

float max(float a, float b) { return (a > b) ? a : b; }
float max(const vec2& v) { return v.maxCoeff(); }
auto max(const vec2& v1, const vec2& v2) {
	return v1.binaryExpr(v2, [](auto a, auto b){ return max(a,b); });
}

float lerp(float a, float b, float t) {
	return a*(1-t) + b*t;
}

float clamp(float value, float x0, float x1) {
	return max(x0, min(x1, value));
}

float smoothstep(float value, float x0, float x1) {
	float t = clamp((value-x0) / (x1-x0), 0, 1);
	return t * t * (3 - 2*t);
}

// https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.30.pdf
// 8 (Built-in Functions)
float radians(float degrees)  { return degrees * (TWO_PI/360.0); }
float degrees(float radians)  { return radians * (360.0/TWO_PI); }
float atan (float y, float x) { return std::atan2(y,x); }
float pow  (float x, float y) { return std::pow(x,y); }
float sign (float x)          { return (x > 0) ? 1 : (x < 0 ? -1 : 0); }
float fract(float x)          { return x - floor(x); }
float mod  (float x, float y) { return x - y*floor(x/y); }

#define unary_func(FUNC) \
	template<typename D> \
	auto FUNC(const Eigen::ArrayBase<D>& V) \
		{ return V.unaryExpr([](auto a){ return FUNC(a); }); }

#define binary_func(FUNC) \
	template<typename D> \
	auto FUNC(const Eigen::ArrayBase<D>& V1, const Eigen::ArrayBase<D>& V2) \
		{ return V1.binaryExpr(V2, [](auto a, auto b){ return FUNC(a,b); }); }

// every one of these listed functions
// will also work when applied to vec2's
unary_func(sin)  	using std::sin;
unary_func(cos)  	using std::cos;
unary_func(tan)  	using std::tan;
unary_func(asin) 	using std::asin;
unary_func(acos) 	using std::acos;
unary_func(atan) 	using std::atan;
unary_func(exp)  	using std::exp;
unary_func(log)  	using std::log;
unary_func(sqrt) 	using std::sqrt;
/*unary_func(abs)*/	using std::abs;
unary_func(floor)	using std::floor;
unary_func(round)	using std::round;
unary_func(ceil) 	using std::ceil;

unary_func(radians)
unary_func(degrees)
unary_func(sign)
unary_func(fract)

binary_func(min)
binary_func(max)
binary_func(atan)
binary_func(pow)
binary_func(mod)
#undef unary_func
#undef binary_func

template<typename D>
auto clamp(const Eigen::ArrayBase<D>& V, float x0, float x1)
	{ return V.unaryExpr([=](auto a){ return clamp(a, x0, x1); }); }
template<typename D>
auto smoothstep(const Eigen::ArrayBase<D>& V, float x0, float x1)
	{ return V.unaryExpr([=](auto a){ return smoothstep(a, x0, x1); }); }
template<typename D>
auto lerp(const Eigen::ArrayBase<D>& V1, const Eigen::ArrayBase<D>& V2, float t)
	{ return V1.binaryExpr(V2, [=](auto a, auto b){ return lerp(a, b, t); }); }



/* color math */
namespace Blend {
	vec3 Normal(const vec3& col1, const vec3& col2, float a=1.0) {
		return lerp(col1, col2, a);
	}
	vec3 Dissolve(const vec3& col1, const vec3& col2, float a=1.0) {
		return (std::rand() > a*RAND_MAX) ? col1 : col2;
	}



	vec3 Add(const vec3& col1, const vec3& col2) {
		return col1 + col2;
	}

	vec3 Multiply(const vec3& col1, const vec3& col2) {
		return col1 * col2;
	}

	vec3 Darken(const vec3& col1, const vec3& col2) {
		return min(col1, col2);
	}

	vec3 Lighten(const vec3& col1, const vec3& col2) {
		return max(col1, col2);
	}

	vec3 Difference(const vec3& col1, const vec3& col2) {
		return col1 - col2;
	}

	vec3 Divide(const vec3& col1, const vec3& col2) {
		return col1 / col2;
	}

	vec3 Normal  (const vec3& col1, const vec4& col2) { return Normal  (col1, col2({0,1,2}), col2(3)); }
	vec3 Dissolve(const vec3& col1, const vec4& col2) { return Dissolve(col1, col2({0,1,2}), col2(3)); }

	#define shorthand(FUNC) \
		vec3 FUNC(const vec3& col1, const vec3& col2, float a) { return Normal(col1, FUNC(col1, col2), a); } \
		vec3 FUNC(const vec3& col1, const vec4& col2)          { return FUNC(col1, col2({0,1,2}), col2(3)); }

	shorthand(Add);
	shorthand(Multiply);
	shorthand(Darken);
	shorthand(Lighten);
	shorthand(Difference);
	shorthand(Divide);
	#undef shorthand
}



/* misc */

// time functions
typedef std::chrono::system_clock::time_point timePoint;
float calcDuration(timePoint t1, timePoint t0) {
	return std::chrono::duration<float>(t1 - t0).count();
}
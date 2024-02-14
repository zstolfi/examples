#pragma once
#include "math.hh"
#include "window.hh"

/* SDFs */
float SDFpoint(const vec2& p) {
	return len(p);
}

float SDFlineOrigin(const vec2& p, const vec2& a) {
	float t0 = clamp(dot(a,p)/dot2(a), 0, 1);
	return len(p - a*t0);
}



float SDFcircle(const vec2& p, float r) {
	return SDFpoint(p) - r;
}

float SDFline(const vec2& p, const vec2& a, const vec2& b) {
	return SDFlineOrigin(p-b, a-b);
}

float SDFbox(const vec2& p, const vec2& r) {
	vec2 p0 = abs(p);
	//     dist-out            dist-in
	return len(p0 - min(p0,r)) + min(0.0, max(p0-r));
}

float SDFplane(const vec2& p, const vec2& n) {
	return sign(dot(p,n)) * len(n * dot(n,p)/dot2(n));
}

float SDFline_s(const vec2& p, const vec2& a, const vec2& b) {
	return sign(dot(p-a,perp(b-a))) * SDFline(p,a,b);
}



/* SDF helper functions */
vec4 SDF_draw(float dist, State& u) {
	vec3 col = (dist < 0.0) ? vec3(0.27, 0.54, 0.91) : vec3(1.0, 1.0, 1.0);
	col *= 1.0 - exp(-6.0*abs(dist));
	col *= 0.9 + 0.1 * cos(150 * dist);
	// col *= 0.9 + 0.1 * cos(150 * 10*log(abs(dist)/10 + 1));
	col += 1.0-smoothstep(abs(dist), 0.5*u.pix, 2.0*u.pix);
	
	return vec4(col(0),col(1),col(2), 1.0);
}

vec4 SDF_drawMouse(const vec2& p, float dist, const vec2& mouse, State& u) {
	// bool circle = len(p-mouse) - dist;
	bool circle = abs(SDFcircle(p-mouse, abs(dist))) - 2*u.pix < 0;
	bool center = SDFcircle(p-mouse, 4*u.pix)                  < 0;

	// vec3 col = dist > 0 ? vec3(1.0, 0.0, 0.0) : vec3(0.0, 1.0, 1.0);
	vec3 col = vec3(1.0, 0.0, 0.0);
	if (dist < 0) col = 1-col;
	return vec4(col(0),col(1),col(2), (circle || center) ? 0.7 : 0.0);
}



/* objects */
// struct Primitive {
// 	float (*base)(float)
// };
// struct Object {
// 	mat3 affine = mat3::Identity(); // transformation matrix
// 	float (*base)(float) = SDFpoint; // function-pointer to SDF

// 	float SDF(vec2 p) {
// 		return base(affine * p);
// 	}
// };
/*    This renders the image!    */
#pragma once
#include <array>
#include "math.hh"
#include "SDFs.hh"
#include <vector>

// TODO: make a system of objects!

float centerShape(const vec2& p) {
	// return SDFcircle(p, 0.5);
	return SDFline_s(abs(p), vec2(0.6, 0.0), vec2(0.0, 0.3));
}

float globalDist(const vec2& p) {
	// float dist = INF;
	// for (int i=0; i < objects.size(); i++) {
	// 	dist = min(dist, objects[i]->SDF(p));
	// }
	// return dist;
	return centerShape(p);
}

vec3 plot(vec2 Coord, State& u) {
	vec2 uv =    (Coord   / u.resolution - 0.5) * u.aspectRatio;
	vec2 mouse = (u.mouse / u.resolution - 0.5) * u.aspectRatio;

	float dist = globalDist(uv);
	vec3 color = SDF_draw(dist,u)({0,1,2});

	if (u.mouseDown(0)) {
		float mDist = globalDist(mouse);
		color = Blend::Normal(color, SDF_drawMouse(uv, mDist, mouse, u));
	}

	return color;
}

// vec3 plot(int Tx, int Ty, State& u) {
// 	vec2 Coord(Tx,Ty);

// 	vec2 st = Coord.array()/u.resolution.array();
// 	vec3 color = vec3(st(0),st(1),abs(sin(u.time)));

// 	return color;
// }

// vec3 plot(int Tx, int Ty, State& u) {
// 	int x = Tx/* - u.mouse[0]*/;
// 	int y = Ty/* - u.mouse[1]*/;

// 	int v = x^y;
// 	return vec3(v/255.0, v/255.0, v/255.0);
// }

void drawPicture(SDL_Surface* img, State& state) {
	int* buffer = (int*)img->pixels;

	for (int y=0; y < img->h; y++) {
	for (int x=0; x < img->w; x++) {
		vec2 Coord = vec2(x, img->h-1 - y);
		
		vec3 col = plot(Coord, state);
		col = clamp(col, 0, 1);
		ivec3 RBG = (col*255).cast<int>();
		buffer[y * img->w + x] = RBG(0)<<16 | RBG(1)<<8 | RBG(2);
	} }
}
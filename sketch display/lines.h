#pragma once
#include <math.h>
#define MAX_LINES (16*1024)

struct line { short x0, y0, x1, y1; } lines[MAX_LINES];
unsigned numLines = 0;


/* graphics */
float clamp(float x, float a, float b) {
	return fmax(a, fmin(b, x));
}

float draw_line(int x, int y, struct line l, int cap0, int cap1) {
	if (l.x0 == l.x1 && l.y0 == l.y1) { return 1; }
	float px =    x - l.x0;
	float py =    y - l.y0;
	float bx = l.x1 - l.x0;
	float by = l.y1 - l.y0;

	float len = hypot(bx,by);
	float a = cap0 ? 1/len     : 0;
	float b = cap1 ? 1 - 1/len : 1;
	float h = clamp((px*bx + py*by)/(bx*bx + by*by), a, b);
	return clamp(hypot(px - h*bx, py - h*by) - 1, 0, 1);
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

struct line line_bounds(struct line l) {
	return (struct line) {
		.x0 = MAX(  0, MIN(l.x0, l.x1) - 3),
		.y0 = MAX(  0, MIN(l.y0, l.y1) - 3),
		.x1 = MIN(800, MAX(l.x0, l.x1) + 4),
		.y1 = MIN(600, MAX(l.y0, l.y1) + 4)
	};
}
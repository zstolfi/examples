#include <stdio.h>
#include <SDL2/SDL.h>
#include "error.h"

/* SDL stuff */
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
unsigned* pixels = NULL;

int init_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
		return error("SDL uninitialized.");
	window = SDL_CreateWindow("Sketch Viewer",
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          800, 600,
	                          0);
	if (!window)
		return error("Window uninitialized.");

	surface = SDL_GetWindowSurface(window);
	if (!surface)
		return error("Surface could not be created.");

	pixels = (unsigned*)surface->pixels;
	return 0;
}

void delete_window(void) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}


/* Sketch stuff */
int parse_args(int argc, char* argv[]) {
	if (argc <= 1)
		return error("No sketch file provided.");
	FILE* fp = fopen(argv[1], "r");
	if (!fp)
		return error("Sketch file could not be opened.");

	for (int c; (c = fgetc(fp)) != EOF; ) { putchar(c); }
	return 0;
}



#define MAX_LINES (16*1024)

struct line { short x0, y0, x1, y1; } lines[MAX_LINES];
unsigned numLines = 0;

void set_pixel(int x, int y, unsigned c) {
	pixels[y * surface->w + x] = c;
}

unsigned get_pixel(int x, int y) {
	return pixels[y * surface->w + x];
}

int main(int argc, char* argv[]) {
	if (parse_args(argc, argv) != 0) { return 1; }
	if (init_window() != 0) { return 2; }
	int running = 1;

	for (int y=0; y<surface->h; y++) {
	for (int x=0; x<surface->w; x++) {
		/*  background color  */
		unsigned c = 0xFFFFFFFF;
		set_pixel(x,y,c);
	} }
	SDL_UpdateWindowSurface(window);

	while (running) {
		for (SDL_Event event; SDL_PollEvent(&event); ) {
			if (event.type == SDL_QUIT) { running = 0; }
		}
	}

	delete_window();
	return 0;
}
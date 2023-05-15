#include <stdio.h>
#include <SDL2/SDL.h>
#include "error.h"

/* SDL stuff */
SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
unsigned* pixels = NULL;

int init_window(void) {
	printf("Creating window...\n");
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
#define MAX_LINES (16*1024)

struct line { short x0, y0, x1, y1; } lines[MAX_LINES];
unsigned numLines = 0;

short val(char c) {
	return ('0' <= c&&c <= '9')
		? c - '0'
		: c - 'a' + 10;
}

short dec(char buf[3]) {
	return 36*val(buf[0]) + val(buf[1]);
}

int parse_args(int argc, char* argv[]) {
	if (argc <= 1)
		return error("No sketch file provided.");
	FILE* fp = fopen(argv[1], "r");
	if (!fp)
		return error("Sketch file could not be opened.");

	printf("Decoding sketch...\n");
	char buf[3]; /* 2 chars */
	int c;
	ungetc(' ', fp);

	short x,y;
	while (!feof(fp)) {
		c = fgetc(fp);
		if (c == EOF) { break; }
		if (c == '|') { /*break;*/ }
		if (c == ' ') {
			x = dec(fgets(buf, 3, fp));
			y = dec(fgets(buf, 3, fp));
		} else {
			ungetc(c, fp);
			short x0 = x, y0 = y;
			lines[numLines++] = (struct line) {
				.x0 = x0,
				.y0 = y0,
				.x1 = x = dec(fgets(buf, 3, fp)),
				.y1 = y = dec(fgets(buf, 3, fp))
			};
		}
	}

	printf("%d lines.\n", numLines);
	for (int i=0; i<numLines; i++) {
		printf("lines[%d]:\t", i);
		printf("\tx0: %d, y0: %d, x1: %d, y1: %d\n", lines[i].x0, lines[i].y0, lines[i].x1, lines[i].y1);
	}

	fclose(fp);
	return 0;
}



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
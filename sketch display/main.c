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
#include "lines.h"

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

	// printf("%d lines.\n", numLines);
	// for (int i=0; i<numLines; i++) {
	// 	printf("lines[%d]:\t", i);
	// 	printf("\tx0: %d, y0: %d, x1: %d, y1: %d\n", lines[i].x0, lines[i].y0, lines[i].x1, lines[i].y1);
	// }

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
	int frameCount = 0;

	/* setup */
	for (int i=0; i<800*600; i++) {
		/*  background color  */
		unsigned c = 0xFFFFFFFF;
		set_pixel(i%800, i/800, c);
	}

	/* draw */
	while (running) {
		for (SDL_Event event; SDL_PollEvent(&event); ) {
			if (event.type == SDL_QUIT) { running = 0; }
		}
		

		for (int j=4*frameCount; j<4*(frameCount+1) && j<numLines; j++) {
			// printf("drawing line #%d...\n", j);
			struct line bounds = line_bounds(lines[j]);
			int cap0 = j>0;
			int cap1 = j<numLines-1;
			cap0 = cap0 && lines[j-1].x1 == lines[j].x0 && lines[j-1].y1 == lines[j].y0;
			cap1 = cap1 && lines[j+1].x0 == lines[j].x1 && lines[j+1].y0 == lines[j].y1;
			for (int y=bounds.y0; y<bounds.y1; y++) {
			for (int x=bounds.x0; x<bounds.x1; x++) {
				unsigned char c = draw_line(x,y, lines[j], !cap0, !cap1)*255;
				unsigned char cOld = (get_pixel(x,y) >> 16) & 0xFF;
				c = MIN(c, cOld);
				set_pixel(x,y, 0xFF<<24 | c<<16 | c<<8 | c);
			} }
		}
		frameCount++;
		SDL_UpdateWindowSurface(window);
		SDL_Delay(1000/60);
	}

	delete_window();
	return 0;
}
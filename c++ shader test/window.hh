/*    Initializes and updates the window    */
#pragma once
#include <SDL.h>
#include "math.hh";

struct State {
	int frameNumber;
	float time;
	float Δtime;
	vec2 resolution;
	vec2 mouse;
	vec2 mouseDown;

	vec2 aspectRatio;
	float pix;
};

class Window {
private:
	char* title;
	int W;
	int H;
	SDL_Window* window;

public:
	SDL_Surface* surface;

	Window(char* title_, const int W_, const int H_) {
		title = title_;
		W = W_;  H = H_;
	}

	bool init() {
		// start video
		if (SDL_Init( SDL_INIT_VIDEO ) != 0) { return false; }

		// make window
		window = SDL_CreateWindow(title ,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H ,
			SDL_WINDOW_SHOWN);
		if (window == NULL)  return false;

		surface = SDL_GetWindowSurface(window);
		// SDL_CaptureMouse(SDL_TRUE);
		return true;
	}

	bool loadMedia() {
		// currently nothing to load
		return true;
	}

	void update() {
		SDL_UpdateWindowSurface(window);
	}

	void close() {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};
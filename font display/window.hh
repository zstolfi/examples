#pragma once
#include <SDL.h>
#include "draw.hh"

class Window {
private:
	std::string title;
	const int W;
	const int H;
	SDL_Window* window;
	SDL_Surface* surface;

public:
	Canvas* canvas;

	Window(std::string title, const int W, const int H)
	: title{title}, W{W}, H{H} {}

	bool init() {
		// start video
		if (SDL_Init(SDL_INIT_VIDEO)) { return false; }

		// make window
		window = SDL_CreateWindow(title.c_str(),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H,
			SDL_WINDOW_SHOWN);
		if (window == NULL)  return false;

		surface = SDL_GetWindowSurface(window);
		canvas = new Canvas(surface);
		// SDL_CaptureMouse(SDL_TRUE);
		return true;
	}

	bool loadMedia() {
		// currently nothing to load
		return true;
	}

	void draw() {
		canvas->draw();
		SDL_UpdateWindowSurface(window);
	}

	void close() {
		delete canvas;
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};
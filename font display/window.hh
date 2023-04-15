#pragma once
#include <SDL.h>
#include <memory>
#include <vector>
#include <fstream>
#include "game state.hh"
#include "draw.hh"
#include "fonts.hh"

class Window {
private:
	std::string title;
	const int W;
	const int H;
	SDL_Window* window;
	SDL_Surface* surface;

public:
	std::unique_ptr<Canvas> canvas;
	Media media;

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
		canvas = std::make_unique<Canvas>(surface);
		// SDL_CaptureMouse(SDL_TRUE);
		return true;
	}

	bool loadMedia() {
		std::ifstream file {"font 2.txt"};
		if (!file) { return false; }
		while (auto font = parseFont(file)) {
			media.fonts.push_back(*font);
		}
		return true;
	}

	void draw(const GameState& s) {
		canvas->draw(media, s);
		SDL_UpdateWindowSurface(window);
	}

	void close() {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};
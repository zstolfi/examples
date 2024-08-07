#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <vector>
#include <span>
using namespace std::literals;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

class Window {
private:
	SDL_Window* m_window = nullptr;

public:
	SDL_Surface* surface = nullptr;
	std::vector<SDL_Surface*> media;

	Window(
		std::string title,
		unsigned W, unsigned H,
		std::span<const std::string> mediaFiles = {}
	) {
		// Start video and image library.
		auto sdlFlags = SDL_INIT_VIDEO;
		auto imgFlags = IMG_INIT_PNG;
		if (SDL_Init(sdlFlags) != 0) throw "SDL init video"s;
		if ((~IMG_Init(imgFlags) & imgFlags) != 0) throw "SDL image init"s;

		// Load media.
		for (std::string path : mediaFiles) {
			SDL_Surface* image = IMG_Load(path.c_str());
			if (!image) throw path;
			media.push_back(image);
		}

		// Make window.
		m_window = SDL_CreateWindow(
			title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			W, H, SDL_WINDOW_SHOWN
		);
		if (!m_window) throw "SDL create window"s;

		// Set our drawing surface.
		surface = SDL_GetWindowSurface(m_window);
	}

	~Window() {
		for (SDL_Surface* s: media) SDL_FreeSurface(s);
		SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	void update() {
		// Display pixels stored in 'surface'.
		SDL_UpdateWindowSurface(m_window);
	}
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

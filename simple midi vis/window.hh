/*    Initializes and updates the window    */
#pragma once
#include <SDL.h>
#include "timeTypes.hh"

// time functions
#include <chrono>
using std::chrono::system_clock;
typedef system_clock::time_point timePoint;
double calcDuration(timePoint t1, timePoint t0) {
	return std::chrono::duration<double>(t1 - t0).count();
}

struct State {
	int frameNumber;
	double time;
	double Δtime;
	// vec2 resolution;
	// vec2 mouse;
	// vec2 mouseDown;

	// vec2 aspectRatio;
	// float pix;

private:
	timePoint startTime, time0, time1;
	float offset;
public:
	void TimerStart(float _offset) {
		offset = _offset;
		startTime = system_clock::now();
		time0 = startTime;
		time1 = startTime;

		time  = calcDuration(time1 , startTime);
		Δtime = calcDuration(time1 , time0);

		frameNumber = 0;
	}

	float getΔtime() { return calcDuration(time1 , time0); }

	void TimerUpdate() {
		time1 = system_clock::now();
		time  = calcDuration(time1 , startTime) + offset;
		Δtime = calcDuration(time1 , time0);

		time0 = time1;
		frameNumber++;
	}
};



class Window {
private:
	std::string title;
	int W;
	int H;
	SDL_Window* window;

public:
	SDL_Surface* surface;

	Window(std::string title, const int W, const int H)
	: title(title), W(W), H(H) {}

	bool init() {
		// start video
		if (SDL_Init(SDL_INIT_VIDEO)) { return false; }

		// make window
		window = SDL_CreateWindow(title.c_str() ,
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
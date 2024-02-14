/*    Simple per-pixel test    */
/*         (using SDL)         */
#include <SDL.h>
#include <chrono>
#include "window.hh"
#include "draw.hh"
#include "math.hh"
#include "SDFs.hh"

using std::chrono::system_clock;

const int W = 768;
const int H = 512;

int main(int argc, char* args[]) {
	Window window("SDL render tests", W, H);
	if (!window.init()     )  return 1;
	if (!window.loadMedia())  return 2;

	timePoint startTime = system_clock::now();
	timePoint time0 = startTime;
	timePoint time1 = startTime;

	State state;
	state.frameNumber = 0;
	state.resolution  = vec2(W,H);
	state.mouse       = vec2(0,0);
	state.mouseDown   = vec2(0,0);

	vec2 ratio = state.resolution({0,1}) / state.resolution({1,0});
	state.aspectRatio = 2.0*max(ratio, vec2(1.0,1.0));
	state.pix = 1.0/min(state.resolution);

	// initObjects();

	/* MAIN LOOP */
	for (bool quit=false; !quit; ) {
		// update state
		time1 = system_clock::now();
		state.time  = calcDuration(time1 , startTime);
		state.Δtime = calcDuration(time1 , time0);

		// handle events
		for(SDL_Event ev; SDL_PollEvent(&ev); ) {
			switch(ev.type) {
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_MOUSEMOTION:
					state.mouse << ev.motion.x, H-1 - ev.motion.y;
					break;
				case SDL_MOUSEBUTTONDOWN:
					/**/ if (ev.button.button == SDL_BUTTON_LEFT )  state.mouseDown(0) = 1.0;
					else if (ev.button.button == SDL_BUTTON_RIGHT)  state.mouseDown(1) = 1.0;
					break;
				case SDL_MOUSEBUTTONUP:
					/**/ if (ev.button.button == SDL_BUTTON_LEFT )  state.mouseDown(0) = 0.0;
					else if (ev.button.button == SDL_BUTTON_RIGHT)  state.mouseDown(1) = 0.0;
					break;
		} }

		drawPicture(window.surface, state);
		window.update();

		// SDL_Delay(1000/60);
		time0 = time1;
		state.frameNumber++;
	}

	window.close();
	return 0;
}
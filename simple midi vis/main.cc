#include <string>
#include <vector>
#include <numeric>
#include "midiImport.hh"
#include "midiRead.hh"
#include "timeTypes.hh"
#include "window.hh"
#include "draw.hh"

std::string filePath = "Prelude in C.mid";
// std::string filePath = "Pipe_Dream (MIDI) v6.mid";
// std::string filePath = "D:/Downloads/midi/black midi.mid";

const int W = 800;
const int H = 600;

void parseArgs(int argc, char* args[]) {
	if (argc > 1) { filePath = args[1]; }
}

int main(int argc, char* args[]) {
	parseArgs(argc, args);

	MidiDataImport importer(filePath);
	// importer.printFile = true;
	importer.read();
	// importer.printTrack(0);
	// return 0;

	Song song(importer.data);

	Window window("Simple Midi Vis", W, H);
	if (!window.init())       return 1;
	if (!window.loadMedia())  return 2;
	Draw::setSurface(window.surface);
	Draw::setFontSize(3);

	State gameState;
	gameState.TimerStart(-1);

	std::array<int,16> channelsToDraw{};
	std::iota(channelsToDraw.begin(), channelsToDraw.end(), 0);

	/* MAIN LOOP */
	for (bool quit=false; !quit; ) {
		// handle events
		for (SDL_Event ev; SDL_PollEvent(&ev); ) {
			switch (ev.type) {
				case SDL_QUIT:  quit = true;  break;
				case SDL_KEYDOWN:
					switch (ev.key.keysym.sym) {
						case SDLK_ESCAPE:  quit = true;  break;

						case SDLK_UP:    Draw::hwySpeed += 0.5;  break;
						case SDLK_DOWN:  Draw::hwySpeed -= 0.5;  break;
					}
				break;
		} }

		// Draw::hwySpeed = 0.5*gameState.time + 0;

		Draw::mainPicture(song, gameState, channelsToDraw);
		window.update();

		float renderTime = gameState.getΔtime();
		// TODO: figure out proper fps capping
		// SDL_Delay(1000/60);
		gameState.TimerUpdate();
	}

	return 0;
}
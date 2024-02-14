/* This renders the image! */
#pragma once
#include <SDL.h>
#include <map>
#include <vector>
#include <array>
// #include <cstdio>
#include "midiRead.hh"
#include "math.hh"

// template <typename V>
// struct DrawParam {

// };

namespace Draw {
	namespace { // anonymous
		int* buffer;
		int  imgW, imgH;
		auto T = [](auto... args) { return std::make_tuple(args...); };

		const int defCol = 0xFFFFFF;
		const int defColText = 0xED1C24;

		const std::vector<int> KeyColor = {0x777777, 0xFFFFFF};

		// color pallete
		const std::vector<int> channelColors {
			 0xF14639, 0x32C914, 0xFBE149, 0x3686F1 ,
			 0xAF2E23, 0x197E24, 0xC98F26, 0x1E5299 ,
			 0xFB9CDF, 0x303134, 0xFD9121, 0x5EF4E4 ,
			 0x6B1E6B, 0xA0ED91, 0xF7C189, 0x327F7F ,
		};


		std::array<int,128> pianoColors;

		const int PianoKeyX[12] =
			{ 0 , 8 , 13, 24, 27, 40, 48, 54, 64, 68, 80, 82 };
		const int PianoKeyW[12] =
			{ 13, 8 , 14, 8 , 13, 14, 8 , 14, 8 , 14, 8 , 14 };
		const std::vector<byte> PianoKeyOrder { 21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38 ,
		40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74 ,
		76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108 ,
		22, 25, 27, 30, 32, 34, 37, 39, 42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70 ,
		73, 75, 78, 80, 82, 85, 87, 90, 92, 94, 97, 99, 102, 104, 106 };


		unsigned px = 1;
		int (*blendMode)(int,int) = Blend::Normal;

		// max font width
		const int FontWidth = 8;
		const int FontHeight = 6;
		struct charData {
			int width;
			std::vector<unsigned char> data;
		};

		const std::map<char, charData> Font {
			{'\0',{0, {}}} ,
			{'-', {3, {0b0000, 0b0000, 0b0000, 0b0111, 0b0000, 0b0000}}} ,
			{'+', {3, {0b0000, 0b0000, 0b0010, 0b0111, 0b0010, 0b0000}}} ,
			{'.', {1, {0b0000, 0b0000, 0b0000, 0b0000, 0b0001, 0b0001}}} ,
			{' ', {4, {}}} ,

			{'0', {4, {0b0110, 0b1011, 0b1011, 0b1011, 0b1011, 0b0110}}} ,
			{'1', {4, {0b0110, 0b1110, 0b0110, 0b0110, 0b0110, 0b1111}}} ,
			{'2', {4, {0b0110, 0b1011, 0b0011, 0b0110, 0b1100, 0b1111}}} ,
			{'3', {4, {0b1110, 0b0011, 0b0110, 0b0011, 0b0011, 0b1110}}} ,
			{'4', {4, {0b1011, 0b1011, 0b1011, 0b1111, 0b0011, 0b0011}}} ,
			{'5', {4, {0b1111, 0b1000, 0b1110, 0b0011, 0b0011, 0b1110}}} ,
			{'6', {4, {0b0110, 0b1000, 0b1110, 0b1011, 0b1011, 0b0110}}} ,
			{'7', {4, {0b1111, 0b0011, 0b0011, 0b0110, 0b0110, 0b0110}}} ,
			{'8', {4, {0b0110, 0b1011, 0b0110, 0b1011, 0b1011, 0b0110}}} ,
			{'9', {4, {0b0110, 0b1011, 0b1011, 0b0111, 0b0011, 0b0110}}} ,

			{'A', {4, {0b0110, 0b1101, 0b1101, 0b1111, 0b1101, 0b1101}}} ,
			{'B', {4, {0b1110, 0b1101, 0b1110, 0b1101, 0b1101, 0b1110}}} ,
			{'C', {4, {0b0111, 0b1100, 0b1100, 0b1100, 0b1100, 0b0111}}} ,
			{'D', {4, {0b1110, 0b1101, 0b1101, 0b1101, 0b1101, 0b1110}}} ,
			{'E', {4, {0b1111, 0b1100, 0b1110, 0b1100, 0b1100, 0b1111}}} ,
			{'F', {4, {0b1111, 0b1100, 0b1110, 0b1100, 0b1100, 0b1100}}} ,
			{'G', {4, {0b0111, 0b1100, 0b1100, 0b1101, 0b1101, 0b0111}}} ,
			{'H', {4, {0b1101, 0b1101, 0b1111, 0b1101, 0b1101, 0b1101}}} ,
			{'I', {4, {0b1111, 0b0110, 0b0110, 0b0110, 0b0110, 0b1111}}} ,
			{'J', {4, {0b1111, 0b0011, 0b0011, 0b0011, 0b1011, 0b0110}}} ,
			{'K', {4, {0b1101, 0b1101, 0b1110, 0b1101, 0b1101, 0b1101}}} ,
			{'L', {4, {0b1100, 0b1100, 0b1100, 0b1100, 0b1100, 0b1111}}} ,
			{'M', {5, {0b11001,0b11111,0b11101,0b11001,0b11001,0b11001}}} ,
			{'N', {5, {0b11001,0b11101,0b11101,0b11011,0b11001,0b11001}}} ,
			{'O', {4, {0b0110, 0b1101, 0b1101, 0b1101, 0b1101, 0b0110}}} ,
			{'P', {4, {0b1110, 0b1101, 0b1101, 0b1110, 0b1100, 0b1100}}} ,
			{'Q', {4, {0b0110, 0b1101, 0b1101, 0b1101, 0b1101, 0b0110, 0b0001}}} ,
			{'R', {4, {0b1110, 0b1101, 0b1101, 0b1110, 0b1101, 0b1101}}} ,
			{'S', {4, {0b0111, 0b1100, 0b1110, 0b0111, 0b0011, 0b1110}}} ,
			{'T', {4, {0b1111, 0b0110, 0b0110, 0b0110, 0b0110, 0b0110}}} ,
			{'U', {4, {0b1101, 0b1101, 0b1101, 0b1101, 0b1101, 0b1110}}} ,
			{'V', {4, {0b1101, 0b1101, 0b1101, 0b1101, 0b0111, 0b0010}}} ,
			{'W', {5, {0b11001,0b11001,0b11001,0b11101,0b11111,0b01010}}} ,
			{'X', {4, {0b1101, 0b1101, 0b0110, 0b0110, 0b1101, 0b1101}}} ,
			{'Y', {4, {0b1101, 0b1101, 0b1101, 0b0110, 0b0110, 0b0110}}} ,
			{'Z', {4, {0b1111, 0b0011, 0b0111, 0b1110, 0b1100, 0b1111}}} ,
		};
	}

	void setSurface(SDL_Surface* img) {
		buffer = (int*)img->pixels;
		imgW = img->w;
		imgH = img->h;
	}

	void setFontSize(unsigned size) {
		px = size;
	}

	void setBlendMode(auto&& func) {
		blendMode = func;
	}


	/* SIMPLE PRIMITIVES */
	template <bool useBlendMode=false>
	void pixel(int x, int y, int col) {
		int pos = y * imgW + x;
		if (0 <= pos&&pos < imgW*imgH) {
			int& btm = buffer[pos];
			// btm &= 0x00FFFFFF;
			if constexpr (!useBlendMode)  { btm = col; }
			else  { btm = blendMode(btm, col); }
		}
	}


	template <bool blend=false>
	void lineX(int y, int x0, int x1, int col=defColText) {
		for (int x=x0; x < x1; x++)
			pixel<blend>(x,y,col);
	}

	template <bool blend=false>
	void lineY(int x, int y0, int y1, int col=defColText) {
		for (int y=y0; y < y1; y++)
			pixel<blend>(x,y,col);
	}

	template <bool blend=false>
	void rect(int x0, int y0, int w, int h, int col=defColText) {
		for (int y=y0; y <= y0+h; y++) {
		for (int x=x0; x <= x0+w; x++) {
			pixel<blend>(x,y,col);
		} }
	}

	template <bool blend=false>
	void rectOutline(int x0, int y0, int w, int h, int col=defCol) {
		for (int y=y0; y <= y0+h; y++) {
		for (int x=x0; x <= x0+w; x++) {
			int pix = (x0 < x&&x < x0+w
			       &&  y0 < y&&y < y0+h) ? col : 0x000000;
			pixel<blend>(x,y,pix);
		} }
	}


	/* PIXEL TEXT */
	void character(char c, int x0, int y0, int col) {
		auto& data = Font.at(c).data;
		int width  = Font.at(c).width;
		for (std::size_t y=0; y < px*data.size(); y++) {
		for (std::size_t x=0; x < px*width; x++) {
			int j = y/px, i = x/px;
			if (data[j] >> (width-i-1) & 1)
				pixel(x+x0, y+y0, col);
		} }
	}

	void text(std::string text, int x0, int y, int col=defColText) {
		int x = x0;
		for (std::size_t i=0; i < text.length(); i++) {
			char c = text[i];
			if (c == '\n') {x = x0; y += px*(FontHeight+1); }
			if (!Font.contains(c)) { continue; }
			character(c, x, y, col);
			x += px * (Font.at(c).width+1);
		}
	}

	template <typename ...T>
	void write(std::tuple<T...> format, int x, int y, int col=defColText) {
		char str[256];
		auto args = std::tuple_cat(std::make_tuple(str), format);
		std::apply(sprintf, args);
		text(str, x, y, col);
	}

	void writeInt(auto n, int x, int y, int col=defColText) {
		char str[64];
		sprintf(str, "%d", n);
		text(str, x, y, col);
	}

	template <int places=3>
	void writeFloat(auto n, int x, int y, int col=defColText) {
		char str[64];
		sprintf(str, "%.*f", places, n);
		text(str, x, y, col);
	}

	void writeHex(auto n, int x, int y, int col=defColText) {
		char str[64];
		sprintf(str, "%X", n);
		text(str, x, y, col);
	}


	/* PIANO FUNCTIONS */
	int pianoX = 45;
	int pianoY = 500;
	int hwyStart = 80;
	int hwyEnd   = 500;
	float hwySpeed = 3.5;

	bool keyType(int midiNum) { // true = white
		switch (midiNum % 12) {
			case 1: case 3: case 6: case 8: case 10:
				return false;
			default:
				return true;
		}
	}

	void drawKey(int midiNum, int col) {
		int octNum = int(midiNum/12) - 1;
		int octStart = pianoX + 96*octNum + 24-96;
		int height = keyType(midiNum) ? 66 : 40;

		if (col >> 24 & 0xFF == 0xFF) { col = KeyColor[keyType(midiNum) ? 1 : 0]; }

		int n = midiNum % 12;
		// underline for C's
		// if (n == 0 && col == 0xFFFFFF)  col = colorMult(col, 0xFFE0E0);
		rectOutline(octStart + PianoKeyX[n], pianoY ,
			PianoKeyW[n], height, col);
	}

	void setPiano(Song& song, float inTime, auto& channelsToDraw) {
		pianoColors.fill(0xFF000000);
		for (auto i : channelsToDraw) {
			if (song.channels[i].usedNotes.size() == 0) { continue; }
			for (std::size_t j=21; j <= 108; j++) {
				auto& noteTL = song.channels[i].notes[j];
				float val = noteTL.valAt(inTime);
				if (val > 0) {
					pianoColors[j] = blendMode(pianoColors[j], channelColors[i]);
				}
			}
		}
	}

	void drawHighway(Channel& channel, float inTime, int color=defColText) {
		for (int y=hwyStart+1; y < hwyEnd; y++) {
			float time = inTime + remap(y, hwyStart, hwyEnd, hwySpeed, 0);
			for (std::size_t i=21; i <= 108; i++) {
				if (channel.usedNotes.contains(i)
				 && channel.notes[i].valAt(time)) {
					int startX = pianoX + 8*(i-21);
					lineX<true>(y, startX, startX+8, color);
				}
			}
		}

		// auto timeToY = [&](float t) { return remap(t - inTime, 0, hwySpeed, hwyEnd, hwyStart); };

		// for (int i=21; i <= 108; i++) { // for each note
		// 	auto& noteTL = channel.notes[i];
		// 	if (!channel.usedNotes.contains(i)) { continue; } // if it's in the song
		// 	float time0 = inTime;
		// 	float time1 = inTime + hwySpeed;
		// 	auto eventList = noteTL.eventsInRange(time0, time1);

		// 	std::vector<std::pair<int,bool>> updates; // keep track of note on/off's
		// 	updates.push_back({timeToY(time0) , noteTL.valAt(time0)});
		// 	for (int j=0; j < eventList.size(); j++) {
		// 		auto& event = eventList[j];
		// 		updates.push_back({timeToY(event.first) , event.second});
		// 	}
		// 	updates.push_back({timeToY(time1) , noteTL.valAt(time1)});

		// 	int x = pianoX + 8*(i-21);
		// 	for (int j=0; j < updates.size()-1; j++) { // draw them as rects!
		// 		auto& u0 = updates[j]; auto& u1 = updates[j+1];
		// 		if (!u0.second) { continue; }
		// 		int height = std::abs(u1.first - u0.first);
		// 		rect<blend>(x, u1.first, 8, height, color);
		// 	}
		// }
	}

	void mainPicture(Song& song, State& u, auto& channelsToDraw) {
		float Δtime = u.Δtime;
		float  time = u. time;

		// clear screen
		for (int y=0; y < imgH; y++) {
		for (int x=0; x < imgW; x++) {
			buffer[y * imgW + x] = 0xFFFFFFFF;
		} }

		// write the time
		write(T("FPS: %.0f", 1.0/Δtime), 10, 10);
		write(T("         TIME: %.2f", time), 10, 10);
		text("------ ZANDER'S EPIC MIDI READER ------", 139, 52);

		// draw the "highway"
		lineX(hwyStart, 0, imgW, 0x000000);
		lineX(hwyEnd,   0, imgW, 0x000000);

		blendMode = Blend::Overlay;
		for (auto n : channelsToDraw) {
			drawHighway(song.channels[n], time, channelColors[n]);
		}

		// draw piano
		setPiano(song, time, channelsToDraw);
		for (byte note : PianoKeyOrder) {
			drawKey(note, pianoColors[note]);
		}
	}
}

#undef T
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <map>

#include "midiType.hh"

std::string filePath = "prelude in C.mid";

int varLen(std::vector<byte> b) {
	int len = b.size();
	for (int i=0; i < len; i++)
		b[i] &= 0x7F;

	int value = 0;
	for (int i=0; i < len; i++)
		value |= b[i] << 7*(len-1 - i);

	return value;
}

int binary(std::vector<byte> b) {
	int len = b.size();

	int value = 0;
	for (int i=0; i < len; i++)
		value |= b[i] << 8*(len-1 - i);

	return value;
}

std::vector<byte> sub(const std::vector<byte>& B, int i0, int i1) {
	return std::vector<byte>(&B[i0], &B[i1]);
}

std::string readType(const std::vector<byte>& B, int p) {
	std::string type{&B[p], &B[p+4]};
	return type;
}

int readBinary(const std::vector<byte>& B, int p, int len) {
	return binary( sub(B,p,p+len) );
}

int sizeVarLen(const std::vector<byte>& B, int p) {
	int i = 0;
	while (B[p+i] >> 7) { i++; }
	return i+1;
}

int readVarLen(const std::vector<byte>& B, int p) {
	int size = sizeVarLen(B,p);
	return varLen( sub(B, p, p+size) );
}

const std::map<int,int> statusLength {
	{0x8, 2} ,
	{0x9, 2} ,
	{0xA, 2} ,
	{0xB, 2} ,
	{0xC, 1} ,
	{0xD, 1} ,
	{0xE, 2}
};



MidiData data;
// std::vector<int> chunkStarts;

void readChunkData(const std::vector<byte>& B, std::string type, int p, int len) {
	if (type == "MThd") {
		
		data.header.format = readBinary(B, p, 2);
		p += 2;
		data.header.numTracks = readBinary(B, p, 2);
		p += 2;
		int division = readBinary(B, p, 2);
		p += 2;
		data.header.SMPTE = division & (1 << 15);

		if (!data.header.SMPTE) {
			data.header.ticks = division;
		} else { // uses SMPTE
			char timecode = division >> 8;
			byte ticks = division & 0x00FF;
			float FPS;
			switch (timecode) {
				case -24:  FPS = 24;        break;
				case -25:  FPS = 25;        break;
				case -29:  FPS = 30/1.001;  break;
				case -30:  FPS = 30;        break;
			}
			data.header.ticksPerSec = FPS * ticks;
		}

	} else if (type == "MTrk") {
		
		TrackChunk track;

		int end = p+len;
		byte status;
		for (int i=0; p < end; i++) {
			Event event;
			event.pos = p;

			event.time = readVarLen(B,p);
			p += sizeVarLen(B,p);

			if (B[p] >> 7) {
				status = B[p];
				p += 1;
			}

			event.status = status;

			int len = 0;
			if (statusLength.contains(status >> 4)) {
				len = statusLength.at(status >> 4);
			} else {
				if (status == 0xFF) { p += 1; }
				len = readVarLen(B,p);
				p += sizeVarLen(B,p);
			}
			p += len;

			track.events.push_back(event);
		}

		data.tracks.push_back(track);

	}
}



void parseArgs(int argc, char* args[]) {
	if (argc > 1) { filePath = args[1]; }
}



int main(int argc, char* args[]) {
	parseArgs(argc, args);
	std::ifstream input(filePath, std::ios::binary);
	if(input.fail()) { return 1; }
	std::vector<byte> buffer(std::istreambuf_iterator<char>(input), {});

	int p = 0; // pointer

	while (p < buffer.size()) {
		// get the type (MThd or MTrk)
		std::string type = readType(buffer, p);
		p += 4;
		// get its length in bytes
		int len = readBinary(buffer, p, 4);
		p += 4;
		// read the chunk
		readChunkData(buffer, type, p, len);
		// and jump to the next one
		p += len;
	}
	
	std::printf("The Midi (\"%s\")\n", filePath.c_str());
	std::printf("\thas %d tracks contained.\n", data.header.numTracks);
	if (!data.header.SMPTE) {
		std::printf("\tdoes not use an SMPTE format.\n");
		std::printf("\thas a tick-rate of %.1f per quarter note.\n\n", data.header.ticks);
	} else {
		std::printf("\tuses an SMPTE format.\n");
		std::printf("\thas a tick-rate of %.1f per second.\n\n", data.header.ticksPerSec);
	}
	for (int i=0; i < data.tracks.size(); i++) {
		std::printf("Track %d\n", i);
		std::printf("\thas %d events contained.\n\n", data.tracks[i].events.size());
	}

	std::cin.get();
	return 0;
}
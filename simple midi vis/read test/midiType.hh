#pragma once
#include <vector>
typedef unsigned char byte;


/* MIDI type */
struct Event {
	int time;
	byte status;
	int pos;
};

struct Chunk {
	char type[4];
	unsigned length;
};

struct HeaderChunk : Chunk {
	int format;
	int numTracks;
	bool SMPTE;
	float ticks; // use for normal Midi's
	float ticksPerSec; // use if SMPTE
};

struct TrackChunk : Chunk {
	std::vector<Event> events;
};

struct MidiData {
	HeaderChunk header;
	std::vector<TrackChunk> tracks;
};
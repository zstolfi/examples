#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <map>
typedef unsigned char byte;


/* byte vector functions */
struct vectorByte : std::vector<byte> {
private:
	int binary(vectorByte b) {
		std::size_t len = b.size();

		int value = 0;
		for (std::size_t i=0; i < len; i++)
			value |= b[i] << 8*(len-1 - i);

		return value;
	}

	int varLen(vectorByte b) {
		std::size_t len = b.size();
		for (std::size_t i=0; i < len; i++)
			b[i] &= 0x7F;

		int value = 0;
		for (std::size_t i=0; i < len; i++)
			value |= b[i] << 7*(len-1 - i);

		return value;
	}

public:
    void set(std::vector<byte>& args) {
        for (byte b : args) { push_back(b); }
    }

    vectorByte sub(int i0, int i1) {
        std::vector<byte> subVec(&(*this)[i0], &(*this)[i1]);
		vectorByte out; out.set(subVec);
		return out;
	}

	std::string readString(int p, int len) {
		std::string str(&(*this)[p], &(*this)[p+len]);
		// if (str.back() == '\0' && len > 0) { str.pop_back(); } TODO? string sanatation
		return str;
	}

	int readBinary(int p, int len) {
		return binary( sub(p,p+len) );
	}

	int sizeVarLen(int p) {
		int i = 0;
		while ((*this)[p+i] >> 7) { i++; }
		return i+1;
	}

	int readVarLen(int p) {
		int size = sizeVarLen(p);
		return varLen( sub(p, p+size) );
	}



	std::string readString() { return readString(0,size()); }
	int binary() { return binary(*this); }
	int varLen() { return varLen(*this); }
};

/* Raw MIDI data */
enum struct EventType { Midi, SysEx, Meta };

struct EventRaw {
	int Δtime;
	EventType type;
	byte status;
	byte metaType;
	int pos;
	vectorByte data;
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
	std::vector<EventRaw> events;
};

struct MidiData {
	HeaderChunk header;
	std::vector<TrackChunk> tracks;
};



class MidiDataImport {
public:
	MidiDataImport(std::string file) : filePath(file) {};

	MidiData data;
	std::string filePath;
	bool printFile = false;

private:
	const std::map<int,int> statusLength {
		{0x8, 2}, {0x9, 2}, {0xA, 2}, {0xB, 2} ,
		{0xC, 1}, {0xD, 1}, {0xE, 2}
	};

	void readChunkData(vectorByte& chunk, std::string type, int p, int len) {
		if (type == "MThd") {
			
			data.header.format = chunk.readBinary(p, 2);
			p += 2;
			data.header.numTracks = chunk.readBinary(p, 2);
			p += 2;
			int division = chunk.readBinary(p, 2);
			p += 2;
			data.header.SMPTE = division & (1 << 15);

			if (!data.header.SMPTE) {
				data.header.ticks = division;
			} else { // uses SMPTE
				char timecode = division >> 8;
				byte ticks = division & 0x00FF;
				float FPS = 0;
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
			byte status = 0;
			while (p < end) {
				EventRaw event;
				event.pos = p;

				event.Δtime = chunk.readVarLen(p);
				p += chunk.sizeVarLen(p);

				if (chunk[p] >> 7) {
					status = chunk[p];
					p += 1;
				}
				event.status = status;
				event.metaType = chunk[p];
				/**/ if (0x80 <= status&&status <= 0xEF) { event.type = EventType::Midi;  }
				else if (0xF0 <= status&&status <= 0xF7) { event.type = EventType::SysEx; }
				else if (status == 0xFF)                 { event.type = EventType::Meta;  }

				int eventLen = 0;
				if (statusLength.contains(status >> 4)) {
					eventLen = statusLength.at(status >> 4);
				} else {
					if (event.type == EventType::Meta) { p += 1; }
					eventLen = chunk.readVarLen(p);
					p += chunk.sizeVarLen(p);
				}
				event.data = chunk.sub(p, p+eventLen);
				p += eventLen;

				track.events.push_back(event);
			}

			data.tracks.push_back(track);

		}
	}

public:
	void read() {
		std::ifstream input(filePath, std::ios::binary);
		if(input.fail()) { error(1); }
		std::vector<byte> vecBuffer(std::istreambuf_iterator<char>(input), {});
		vectorByte buffer; buffer.set(vecBuffer);

		std::size_t p = 0; // pointer

		while (p < buffer.size()) {
			// get the type (MThd or MTrk)
			std::string type = buffer.readString(p,4);
			p += 4;
			// get its length in bytes
			int len = buffer.readBinary(p,4);
			p += 4;
			// read the chunk
			readChunkData(buffer, type, p, len);
			// and jump to the next one
			p += len;
		}
		
		if (printFile) { print(); }
	}

// private:
	void print() {
		std::printf("The Midi (\"%s\")\n", filePath.c_str());
		std::printf("\thas %d tracks contained.\n", data.header.numTracks);
		if (!data.header.SMPTE) {
			std::printf("\tdoes not use an SMPTE format.\n");
			std::printf("\thas a tick-rate of %.1f per 1/4 note.\n\n", data.header.ticks);
		} else {
			std::printf("\tuses an SMPTE format.\n");
			std::printf("\thas a tick-rate of %.1f per second.\n\n", data.header.ticksPerSec);
		}
		for (std::size_t i=0; i < data.tracks.size(); i++) {
			std::printf("Track %zu\n", i);
			std::printf("\thas %zu events contained.\n\n", data.tracks[i].events.size());
		}
	}

	void printTrack(int n) {
		std::printf("\nThe data of Track %d is:\n", n);
		for (std::size_t i=0; i < data.tracks[n].events.size(); i++) {
			auto event = data.tracks[n].events[i];
			std::string typeStr;
			switch (event.type) {
				case EventType::Midi:  typeStr = "Midi ";  break;
				case EventType::SysEx: typeStr = "SysEx";  break;
				case EventType::Meta:  typeStr = "Meta ";  break;
			}
			std::printf("%s[%02X]: ", typeStr.c_str(), event.status);
			if (event.type == EventType::Meta) {
				std::printf("%02X ", event.metaType);
			}
			for (std::size_t j=0; j < event.data.size(); j++) {
				std::printf("%02X ", event.data[j]);
			}
			printf((i+1)%4 ? "\t\t" : "\n");
		}
	}

	void error(int n) {
		switch (n) {
			case 1:  throw std::invalid_argument("Midi file failed to load.");
		}
	}
};
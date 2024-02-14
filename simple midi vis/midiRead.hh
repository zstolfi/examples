#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <set>
#include "midiImport.hh"
#include "timeTypes.hh"

/* usable Song data */
enum struct MusicMode { Atonal /* atonal meaning don't display any keySig*/ ,
	Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian ,
	Major = Ionian, Minor = Aeolian };
struct KeySig {
	char key; // n => n sharps. -n => n flats
	MusicMode mode;
};

struct TimeSig {
	byte num;
	byte den;	
};

struct Marker {
	double time;
	std::string text;
};

struct NoteVal {
	float velocity;
	byte  pitch;
	double duration = -1;
	float velocityOff;
};

struct Channel {
	std::string name;
	// old version of notes
	std::array<TimeLine<double,float>, 128> notes;
	std::set<byte> usedNotes;
	TimeLine<double,KeySig>  keySig;
	TimeLine<double,TimeSig> timeSig;
};

struct Track {
	std::string name;
	// new way of handling notes
	TimeLine<double,NoteVal> notes;
	std::set<byte> usedNotes;
	std::vector<std::string> comments;
	double length;
};

class Song {
private:
	std::map<std::string,int> nameToTrackIndex;
public:
	std::vector<Track> tracks;
	std::array<Channel,17> channels; // cannels[16] is the global channel
	TimeLine<double,TimeSig> timeSig;

	std::string copyright;
	std::vector<Marker> markers;

	Song(MidiData& midi) {
		tickRate = !midi.header.SMPTE ? midi.header.ticks : midi.header.ticksPerSec;

		// run through all the tempo events first
		for (TrackChunk& midiTrack : midi.tracks) {
			int ticks = 0;
			int channelPrefix = 16;
			for (EventRaw& event : midiTrack.events) {
				ticks += event.Δtime;
				if (event.type != EventType::Meta) { continue; }
				switch (event.metaType) {
					case 0x20:  channelPrefix = event.data[0];  break;

					// I permit any chanels to have their own independent
					// KeySig or TimeSig tracks. Though the default channel
					// will be #16 (the global channel)

					// tempo
					case 0x51: {
						// (microseconds per 1/4 note)
						int micro = event.data.binary();
						tempoList.set(ticks, micro);
						// std::cout << ticks << "\t" << micro << "\t\t\t\t tempo :D" << std::endl;
					} break;
					// SMPTE offset
					case 0x54:  break;
					// time signature
					case 0x58: {
						TimeSig ts {event.data[0], 1 << event.data[1]};
						timeSigRaw[channelPrefix].set(ticks, ts);
					} break;
					// key signature
					case 0x59:
						MusicMode mode;
						/**/ if (event.data[1] == 0) { mode = MusicMode::Major; }
						else if (event.data[1] == 1) { mode = MusicMode::Minor; }

						KeySig ks {static_cast<char>(event.data[0]), mode};
						keySigRaw[channelPrefix].set(ticks, ks);
					break;
		} } }
		processTempoData();

		// read events
		for (TrackChunk& midiTrack : midi.tracks) {
			Track track;
			int totalTicks = 0;
			int channelPrefix = 16; // default to global channel
			std::map<byte,NoteVal> lastNotes;
			std::map<byte,double>  lastNotesTime;

			for (EventRaw& event : midiTrack.events) {
				totalTicks += event.Δtime;
				double time = tickToSec(totalTicks);

				switch (event.type) {
					case EventType::Midi: {
						int action  = event.status >> 4;
						Channel& channel = channels[event.status & 0x0F];
						channelPrefix = 16;

						switch (action) {
							// note off
							case 0x8: {
								int pitch = event.data[0];
								int vel   = event.data[1]; // wtf is release velocity anyway

								// end channel note
								channel.notes[pitch].set(time, 0);

								// end track note
								NoteVal& lastNote = lastNotes.at(pitch);
								lastNote.duration = time - lastNotesTime.at(pitch);
								lastNote.velocityOff = vel/127.0;
								track.notes.setNext(time, lastNote);

								lastNotes.erase(pitch);
								lastNotesTime.erase(pitch);
							} break;
							// note on
							case 0x9: {
								int pitch = event.data[0];
								int vel   = event.data[1];

								// set channel note
								channel.notes[pitch].set(time, vel/127.0);

								// set track note
								if (lastNotes.contains(pitch)) {
									NoteVal& lastNote = lastNotes.at(pitch);
									// imgur
									if (lastNote.duration < 0) {
										lastNote.duration = time - lastNotesTime.at(pitch);
										lastNote.velocityOff = 1.0;
										track.notes.setNext(time, lastNote);
									}
								}
								NoteVal newNote;
								newNote.velocity = vel/127.0;
								newNote.pitch = pitch;

								lastNotes[pitch] = newNote;
								lastNotesTime[pitch] = time;
								
								channel.usedNotes.insert(pitch);
								track.usedNotes.insert(pitch);
							} break;
							// pressure (note)
							case 0xA:  break;
							// controller
							case 0xB:  break;
							// program change
							case 0xC:  break;
							// pressure (channel)
							case 0xD:  break;
							// pitch bend
							case 0xE:  break;
						}

					} break;

					case EventType::SysEx: {

						// Don't do anything really.
						// But if you wanted to insert any data
						// here is where you'd place it.

					} break;

					case EventType::Meta: {

						switch (event.metaType) {
							// text
							case 0x01:
								track.comments.push_back(event.data.readString());
							break;
							// copyright
							case 0x02:
								copyright = event.data.readString();
							break;
							// track name
							case 0x03:
								track.name = event.data.readString();
								nameToTrackIndex.insert({track.name, tracks.size()});
							break;
							// instrument name
							case 0x04:
								channels[channelPrefix].name = event.data.readString();
							break;
							// marker OR cue point
							case 0x06: case 0x07: {
								std::string text = event.data.readString();
								markers.push_back(Marker{time,text});
							} break;
							// select channel prefix
							case 0x20:
								channelPrefix = event.data[0];
							break;
							// end of track
							case 0x2F:
								track.length = time;
							break;
						}

					} break;
				}
			}

			tracks.push_back(track);
		}

		// additional channel info
		for (Channel& channel : channels) {
			for (std::size_t i=0; i < 128; i++) {
				auto& noteTrack = channel.notes[i];
				if (!noteTrack.empty()) {
					double startTime = noteTrack.start();
					noteTrack.set(startTime - 1, 0);
				}
		} }
	}

	 bool  hasTrack(std::string name) { return nameToTrackIndex.contains(name); }
	Track& getTrack(std::string name) { return tracks[nameToTrackIndex.at(name)]; }

	 bool  hasTrack(std::size_t index) { return index < tracks.size(); }
	Track& getTrack(std::size_t index) { return tracks.at(index); }

private:
	// timing functionality

	float tickRate;
	TimeLine<int,int> tempoList; // each element is {<time in ticks>, <bpm in micros>}
	std::array<TimeLine<int,KeySig> ,17> keySigRaw;
	std::array<TimeLine<int,TimeSig>,17> timeSigRaw;

	// https://www.desmos.com/calculator/uioa3zpeur
	// timeline of x, {y, slope}
	TimeLine<int, std::pair<double,double>> tempoPoints;
	void processTempoData() {
		// setup tickToSec()
		tempoPoints.set(0, {0,0});
		tempoList.iterate([&](int ticks, int micro) {
			int  xPrev              = tempoPoints.end();
			auto [yPrev, slopePrev] = tempoPoints.last();

			int x = ticks;
			double y = slopePrev * (x - xPrev) + yPrev;
			double slope = micro/(1000000 * tickRate);

			tempoPoints.set(x, {y,slope});
		});

		//convert the raw keySig/timeSig data
		for (int i=0; i < 17; i++) {
			channels[i].keySig.setNone({0, MusicMode::Atonal});
			channels[i].timeSig.setNone({4,4});

			keySigRaw[i].iterate([&](int ticks, KeySig ks) {
				channels[i].keySig.set(tickToSec(ticks), ks);
			});

			timeSigRaw[i].iterate([&](int ticks, TimeSig ts) {
				channels[i].timeSig.set(tickToSec(ticks), ts);
			});
		}
	}

	double tickToSec(int t) {
		// assume tempoPoints is set
		auto [ticks,pair] = tempoPoints.pairAt(t);
		auto [secs,rate] = pair;

		return rate * (t - ticks) + secs;
	}
};
#pragma once
#include <SDL2/SDL.h>
#include <cstdint>

namespace SDL {
	uint32_t surfaceGetPixel(SDL_Surface* img, unsigned x, unsigned y) {
		uint8_t* bytes = (uint8_t*)img->pixels;
		bytes += y * img->pitch + x * img->format->BytesPerPixel;

		switch (img->format->BytesPerPixel) {
			case 1: return *bytes;
			case 2: return *(uint16_t*)bytes;
			case 4: return *(uint32_t*)bytes;
			case 3: return (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				? bytes[0] << 16 | bytes[1] << 8 | bytes[2]
				: bytes[2] << 16 | bytes[1] << 8 | bytes[0];
			default: return {};
		}
	}

	void surfaceSetPixel(SDL_Surface* img, unsigned x, unsigned y, uint32_t c) {
		uint8_t* bytes = (uint8_t*)img->pixels;
		bytes += y * img->pitch + x * img->format->BytesPerPixel;

		switch (img->format->BytesPerPixel) {
			case 1: *bytes = c;
			case 2: *(uint16_t*)bytes = c;
			case 4: *(uint32_t*)bytes = c;
			case 3: if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			/**/ { bytes[0] = c >> 16, bytes[1] = c >> 8, bytes[2] = c; }
			else { bytes[2] = c >> 16, bytes[1] = c >> 8, bytes[0] = c; }
		}
	}
};

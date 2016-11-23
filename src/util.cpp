#include <SDL2/SDL.h>

#include "util.hpp"

int16_t toi16(uint8_t _a, uint8_t _b)
{
		return ((int16_t)_b << 8) | _a;
}

void getLargestWinDim(int *_w, int *_h)
{
	SDL_DisplayMode best;
	best.w = 0;
	best.h = 9999;

	for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
	{
		SDL_DisplayMode temp;
		SDL_GetCurrentDisplayMode(i, &temp);
		best.w += temp.w;
		if(temp.h < best.h)
			best.h = temp.h;
	}

	*_w = best.w;
	*_h = best.h;
}

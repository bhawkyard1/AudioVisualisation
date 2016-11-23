#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdint>

int16_t toi16(uint8_t _a, uint8_t _b);

template<class T>
T clamp(T _in, T _lo, T _hi)
{
	if(_in < _lo)
		return _lo;
	else if(_in > _hi)
		return _hi;

	return _in;
}

void getLargestWinDim(int * _w, int * _h);

#endif

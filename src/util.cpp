#include "util.hpp"

int16_t toi16(uint8_t _a, uint8_t _b)
{
		return ((int16_t)_b << 8) | _a;
}

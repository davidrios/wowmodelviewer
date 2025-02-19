#include "animated.h"

_ANIMATED_API_ size_t globalTime = 0;

float frand()
{
	return rand() / static_cast<float>(RAND_MAX);
}

float randfloat(float lower, float upper)
{
	return lower + (upper - lower) * (rand() / static_cast<float>(RAND_MAX));
}

_ANIMATED_API_ int randint(int lower, int upper)
{
	return lower + static_cast<int>((upper + 1 - lower) * frand());
}

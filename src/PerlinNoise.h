#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <array>
#include <cmath>
#include "Texture.h"

class PerlinNoise {
protected:
	// permutation array
	std::array<int, 512> p;
	// fade curve 6t^5 - 15t^4 + 10t^3
	static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	// linear interpolation
	static float lerp(float t, float a, float b) { return fma(t, b, fma(-t, a, a)); }
	// gradient function
	static float grad(int hash, float x, float y, float z);
public:
	PerlinNoise(unsigned int seed = 0);
	float noise(float x, float y, float z) const;
	float octaveNoise(float x, float y, float z, int octaves, float persistence) const;
};

#endif
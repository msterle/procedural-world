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
	static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	// linear interpolation
	static double lerp(double t, double a, double b) { return fma(t, b, fma(-t, a, a)); }
	// gradient function
	static double grad(int hash, double x, double y, double z);
public:
	PerlinNoise(unsigned int seed = 0);
	double noise(double x, double y, double z);
	double octaveNoise(double x, double y, double z, int octaves, double persistence);
	Texture2D* newNoiseTexture(int width, int height);
	Texture2D* newOctaveNoiseTexture(int width, int height, int octaves, double persistence);
};

#endif
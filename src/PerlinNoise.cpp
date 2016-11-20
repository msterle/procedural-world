#include "PerlinNoise.h"

#include <cmath>
#include <array>
#include <numeric>
#include <algorithm>

// debug only
#include <iostream>
using namespace std;

double PerlinNoise::grad(int hash, double x, double y, double z) {
	// using lowest 4 bits of hash
	switch(hash & 0xF) {
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
	}
}

PerlinNoise::PerlinNoise(unsigned int seed) {
	// generate a sequence from 0...255
	std::array<int, 256> ptemp;
	std::iota(ptemp.begin(), ptemp.end(), 0);
	// shuffle according to seed
	std::default_random_engine engine(seed);
	std::shuffle(ptemp.begin(), ptemp.end(), engine);
	// double permutation set in length to prevent overflow
	for(int i = 0; i < 512; ++i)
		p[i] = ptemp[i%256];
}

double PerlinNoise::noise(double x, double y, double z) {
	// first 8 bits of integer parts
	int xi = (int)floor(x) & 255, yi = (int)floor(y) & 255, zi = (int)floor(z) & 255;
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	// get faded distances from coordinates
	double u = fade(x), v = fade(y), w = fade(z);
	// hashing method
	int a = p[xi]     + yi, aa = p[a] + zi, ab = p[a + 1] + zi;
	int b = p[xi + 1] + yi, ba = p[b] + zi, bb = p[b + 1] + zi;
	// find component of each hash for 8 surrounding vertices, normalize on [0,1]
	return (lerp(w, lerp(v, lerp(u, grad(p[aa    ], x    , y    , z    ),
									grad(p[ba    ], x - 1, y    , z    )),
							lerp(u, grad(p[ab    ], x    , y - 1, z    ),
									grad(p[bb    ], x - 1, y - 1, z    ))),
					lerp(v, lerp(u, grad(p[aa + 1], x    , y    , z - 1),
									grad(p[ba + 1], x - 1, y    , z - 1)),
							lerp(u, grad(p[ab + 1], x    , y - 1, z - 1),
									grad(p[bb + 1], x - 1, y - 1, z - 1)))) + 1.0) / 2.0;
}

double PerlinNoise::noise(double x, double y, double z, int octaves, double persistence) {
	double val = 0, frequency = 1, amplitude = 1, maxVal = 0;
}

Texture2D* PerlinNoise::newNoiseTexture(int width, int height) {
	unsigned char data[height][width][4];
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < width; ++y) {
			data[y][x][0] = data[y][x][1] = data[y][x][2] = round(1 + sin((double)x / 3.0 + noise(10.0 * x / width, 10.0 * y / height, 0)) * 255.0 / 2);
			data[y][x][3] = 255;
		}
	}
	Texture2D* noiseTex = new Texture2D(GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	for(int x = 0; x < width / 2; ++x) {
		for(int y = 0; y < width / 2; ++y) {
			data[y][x][0] = data[y][x][1] = data[y][x][2] = round(1 + sin((double)x / 3.0 + noise(10.0 * x / width, 10.0 * y / height, 0)) * 255.0 / 2);
			data[y][x][3] = 255;
		}
	}
	noiseTex->setPixelData(data, {width / 2, width / 2}, {width / 4, height / 4});
	return noiseTex;
}
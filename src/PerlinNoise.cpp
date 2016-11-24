#include "PerlinNoise.h"

#include <cmath>
#include <array>
#include <numeric>
#include <algorithm>
#include <vector>
#include <random>
#include <cmath>

// debug only
#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/CImg.h"
using namespace std;

float PerlinNoise::grad(int hash, float x, float y, float z) {
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
	// float permutation set in length to prevent overflow
	for(int i = 0; i < 512; ++i)
		p[i] = ptemp[i%256];
}

float PerlinNoise::noise(float x, float y, float z, unsigned int period) const {
	// wrap to period
	if(period < 1)
		period = 1;
	x = fmod(x, (float)period);
	y = fmod(y, (float)period);
	z = fmod(z, (float)period);
	// first 8 bits of integer parts
	int xi = (int)floor(x) & 255, yi = (int)floor(y) & 255, zi = (int)floor(z) & 255;
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	// get faded distances from coordinates
	float u = fade(x), v = fade(y), w = fade(z);
	// hashing method
	int aaa = p[p[p[               xi] +                yi] +                zi];
    int aba = p[p[p[               xi] + (yi + 1) % period] +                zi];
    int aab = p[p[p[               xi] +                yi] + (zi + 1) % period];
    int abb = p[p[p[               xi] + (yi + 1) % period] + (zi + 1) % period];
    int baa = p[p[p[(xi + 1) % period] +                yi] +                zi];
    int bba = p[p[p[(xi + 1) % period] + (yi + 1) % period] +                zi];
    int bab = p[p[p[(xi + 1) % period] +                yi] + (zi + 1) % period];
    int bbb = p[p[p[(xi + 1) % period] + (yi + 1) % period] + (zi + 1) % period];
	// find component of each hash for 8 surrounding vertices, normalize on [0,1]
	return (lerp(w, lerp(v, lerp(u, grad(aaa, x    , y    , z    ),
									grad(baa, x - 1, y    , z    )),
							lerp(u, grad(aba, x    , y - 1, z    ),
									grad(bba, x - 1, y - 1, z    ))),
					lerp(v, lerp(u, grad(aab, x    , y    , z - 1),
									grad(bab, x - 1, y    , z - 1)),
							lerp(u, grad(abb, x    , y - 1, z - 1),
									grad(bbb, x - 1, y - 1, z - 1)))) + 1.0) / 2.0;
}

float PerlinNoise::octaveNoise(float x, float y, float z, int octaves, float persistence, 
		unsigned int period) const {
	float sum = 0, frequency = 1, amplitude = 1, maxValue = 0;
    for(int i = 0; i < octaves; ++i) {
        sum += noise(x * frequency, y * frequency, z * frequency, period) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2;
    }
    return sum/maxValue;
}
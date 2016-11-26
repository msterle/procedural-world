#ifndef SEEDER_H
#define SEEDER_H

#include "Terrain.h"
#include "ParaTree.h"
#include "Model.h"
#include "Texture.h"
#include <list>

class Seeder {
protected:
	Terrain* terrain;
	Texture2D* barkTex;
public:
	Seeder(Terrain* terrain, Texture2D* barkTex) : terrain(terrain), barkTex(barkTex) { }
	std::list<Model*> seed(int count);
protected:
	
};

#endif
#ifndef WATER_H
#define WATER_H

#include "Model.h"
#include "Shader.h"

class Water : public Model {
protected:

public:
	Water(float waterLevel = 0);
	void draw(Shader* shader);
}

#endif
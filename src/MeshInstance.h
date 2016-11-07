#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H

#include <vector>

#include "../include/glm/gtc/type_ptr.hpp"

#include "Material.h"

class MeshInstance {
public:
	glm::mat4 instanceMat = glm::mat4(1);
	Material material;
	unsigned int modelIndex, meshId, instanceId;
	MeshInstance(unsigned int modelIndex, unsigned int meshId, Material material);
	MeshInstance* copy();
	MeshInstance* translate(glm::vec3 t);
	MeshInstance* scale(glm::vec3 s);
	MeshInstance* rotate(float angle, glm::vec3 axis);
	static unsigned int idCounter;
};

#endif
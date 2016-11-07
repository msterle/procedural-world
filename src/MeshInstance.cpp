#include "MeshInstance.h"

#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "Mesh.h"
#include "Material.h"
#include "ModelManager.h"

// debug only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"
using namespace std;


//// Static member initializations

unsigned int MeshInstance::idCounter = 0;


//// Constructor

MeshInstance::MeshInstance(unsigned int modelIndex, unsigned int meshId, Material material) {
	this->modelIndex = modelIndex;
	this->meshId = meshId;
	this->material = material;
	this->instanceId = MeshInstance::idCounter++;\
}


//// Public methods

MeshInstance* MeshInstance::copy() {
	return ModelManager::getInstance().newMeshInstance(this);
}

MeshInstance* MeshInstance::translate(glm::vec3 t) {
	this->instanceMat = glm::translate(glm::mat4(1), t) * this->instanceMat;
	return this;
}

MeshInstance* MeshInstance::scale(glm::vec3 s) {
	this->instanceMat = glm::scale(glm::mat4(1), s) * this->instanceMat;
	return this;
}

MeshInstance* MeshInstance::rotate(float angle, glm::vec3 axis) {
	this->instanceMat = glm::rotate(glm::mat4(1), angle, axis) * this->instanceMat;
	return this;
}
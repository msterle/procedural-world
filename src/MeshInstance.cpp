#include "MeshInstance.h"

#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "Mesh.h"
#include "Material.h"


// Constructor

MeshInstance::MeshInstance(Mesh* mesh, Material* material) {
	this->mesh = mesh;
	this->material = material;
}


// Public methods

void MeshInstance::translate(glm::vec3 t) {
	this->instanceMat = glm::translate(glm::mat4(), t) * this->instanceMat;
}

void MeshInstance::scale(glm::vec3 s) {
	this->instanceMat = glm::scale(glm::mat4(), s) * this->instanceMat;
}

void MeshInstance::rotate(float angle, glm::vec3 axis) {
	this->instanceMat = glm::rotate(glm::mat4(), angle, axis) * this->instanceMat;
}
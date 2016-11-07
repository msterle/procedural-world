#include "ModelNew.h"

#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "ModelManager.h"
#include "Material.h"

// debug only
#include <iostream>
using namespace std;


//// Constructors

ModelNew::ModelNew() {
	
}

//// Destructor

ModelNew::~ModelNew() {
	
}


//// Public methods

// Transformation methods

void ModelNew::translate(glm::vec3 t) {
	this->modelMat = glm::translate(glm::mat4(1), t) * this->modelMat;
}

void ModelNew::scale(glm::vec3 s) {
	this->modelMat = glm::scale(glm::mat4(1), s) * this->modelMat;
}

void ModelNew::rotate(float angle, glm::vec3 axis) {
	this->modelMat = glm::rotate(glm::mat4(1), angle, axis) * this->modelMat;
}
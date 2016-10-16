#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <vector>

#include "model.h"

using namespace std;

// Public Methods

Model::Model() {
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->vertexBuffer);
	glGenBuffers(1, &this->indexBuffer);
}

void Model::setShaderProgram(GLuint shaderProgram) {
	this->shaderProgram = shaderProgram;
}

void Model::translate(glm::vec3 t) {
	this->modelMat = glm::translate(glm::mat4(), t) * this->modelMat;
}

void Model::scale(glm::vec3 s) {
	this->modelMat = glm::scale(glm::mat4(), s) * this->modelMat;
}

void Model::rotate(float angle, glm::vec3 axis) {
	this->modelMat = glm::rotate(glm::mat4(), angle, axis) * this->modelMat;
}
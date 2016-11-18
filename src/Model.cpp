#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <vector>

#include "Shader.h"
#include "Material.h"

#include "Model.h"

using namespace std;

// Public Methods

void Model::translate(glm::vec3 t) {
	modelMat = glm::translate(glm::mat4(1), t) * modelMat;
}

void Model::scale(glm::vec3 s) {
	modelMat = glm::scale(glm::mat4(1), s) * modelMat;
}

void Model::rotate(float angle, glm::vec3 axis) {
	modelMat = glm::rotate(glm::mat4(1), angle, axis) * modelMat;
}

void Model::draw(Shader* shader) {
	glm::mat4 normalMat = glm::mat3(glm::transpose(glm::inverse(modelMat)));
	shader->use();
	glUniformMatrix4fv(glGetUniformLocation(shader->getRef(), "modelMat"), 
		1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(glGetUniformLocation(shader->getRef(), "normalMat"), 
		1, GL_FALSE, glm::value_ptr(normalMat));
	for(list<Mesh>::iterator it = meshes.begin(); it != meshes.end(); it++) {
		it->draw(shader, modelMat);
	}
}
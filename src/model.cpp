#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <vector>

using namespace std;

class Model {
public:
	Model();
	void setShaderProgram(GLuint shaderProgram);
	void translate(glm::vec3 t);
	void scale(glm::vec3 s);
	void rotate(float angle, glm::vec3 axis);
protected:
	GLuint VAO, vertexBuffer, indexBuffer;
	GLuint shaderProgram;
	vector<glm::vec3> vertices;
	vector<unsigned int> indices;
	glm::mat4 modelMat;
};

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
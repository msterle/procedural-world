#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "../include/glfw3.h"

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
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 modelMat;
};

#endif
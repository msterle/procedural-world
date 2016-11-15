#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <list>
#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "Vertex.h"
#include "Mesh.h"

class Shader;

class Model {
protected:
	glm::mat4 modelMat;
	list<Mesh> meshes;
public:
	Model() : modelMat(glm::mat4(1)) { }
	Mesh* newMesh(vector<Vertex> vertices, GLenum drawMode = GL_TRIANGLES) {
		meshes.push_back(Mesh(vertices, drawMode));
		return &meshes.back();
	}
	Mesh* newMesh(vector<Vertex> vertices, vector<GLuint> indices, GLenum drawMode = GL_TRIANGLES) {
		meshes.push_back(Mesh(vertices, indices, drawMode));
		return &meshes.back();
	}
	Mesh* newMesh(string filePath) {
		meshes.push_back(Mesh(filePath));
		return &meshes.back();
	}
	glm::mat4 getModelMat() { return modelMat; }
	void translate(glm::vec3 t);
	void scale(glm::vec3 s);
	void rotate(float angle, glm::vec3 axis);
	void draw(Shader shader);
	list<Mesh> meshes getMeshes() { return meshes; }
};

#endif
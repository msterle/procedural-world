#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"

using namespace std;


// debug only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"


class MeshInstancePtr;
class MeshInstance;

class Mesh {
protected:
	bool isIndexed;
	GLenum drawMode;
	GLuint VAO, VBO, EBO, IBO;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<MeshInstance> instances;
	Texture2D* tex;
	void init();
public:
	Mesh(vector<Vertex> vertices, GLenum drawMode = GL_TRIANGLES, Texture2D* tex = NULL);
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, GLenum drawMode = GL_TRIANGLES, Texture2D* tex = NULL);
	Mesh(string filePath, Texture2D* tex = NULL);
	MeshInstancePtr newInstance();
	MeshInstancePtr newInstance(glm::mat4 instanceMat);
	MeshInstancePtr newInstance(Material material);
	MeshInstancePtr newInstance(glm::mat4 instanceMat, Material material);
	MeshInstancePtr newInstance(MeshInstancePtr orig);
	//void deleteInstance(MeshInstancePtr instance);
	void draw(Shader* shader, glm::mat4 modelMat);
	int getNumInstances() { return instances.size(); }
	vector<MeshInstance> getInstances() { return instances; }
	vector<Vertex> getVertices() { return vertices; }
	void setTexture(Texture2D* tex) { this->tex = tex; }
	//go through each instance and calls its respective getInstanceMatirx() method
	vector<glm::mat4> getInstancesMatrices();
};

// turn into template?
class MeshInstancePtr {
public:
	typedef vector<MeshInstance>::size_type size_type;
protected:
	bool isNull;
	vector<MeshInstance>* container;
	size_type offset;
public:
	MeshInstancePtr() : isNull(true) { }
	MeshInstancePtr(vector<MeshInstance>* container, size_type offset) {
		isNull = false;
		this->container = container;
		this->offset = offset;
	}
	MeshInstance operator*();
	MeshInstance* operator->();

};

class MeshInstance {
public:
	MeshInstancePtr parent;
	glm::mat4 instanceMat;
	Material material;
	MeshInstancePtr translate(glm::vec3 t) {
		instanceMat = glm::translate(glm::mat4(1), t) * instanceMat;
		return parent;
	}
	MeshInstancePtr scale(glm::vec3 s) {
		instanceMat = glm::scale(glm::mat4(1), s) * instanceMat;
		return parent;
	}
	MeshInstancePtr rotate(float angle, glm::vec3 axis) {
		instanceMat = glm::rotate(glm::mat4(1), angle, axis) * instanceMat;
		return parent;
	}
	MeshInstancePtr applyMatrix(glm::mat4 transform) {
		instanceMat = transform * instanceMat;
		return parent;
	}
	glm::mat4 getInstanceMat() { return instanceMat; }


};

#endif
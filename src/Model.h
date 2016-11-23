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

using namespace std;

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
	list<Mesh> getMeshes() { return meshes; }
	virtual glm::vec3 getPosition() { return glm::vec3(0,0,0); }//virtual member that base class(eg. ParaTree, Rocks must implement)
	virtual void createBoundingVolume(){ cout << "Unable to create Bounding Volume" << endl; }
	virtual glm::vec3 getCenterCoord() {return glm::vec3(0, 0, 0); }
	virtual vector<float> getBoundingVolumePos(){ //virtual member that base class(eg. ParaTree, Rocks must implement)
		vector<float> empty;
		return empty; }
};

#endif
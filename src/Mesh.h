#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "../include/glew.h"
#include "../include/glfw3.h"

#include "Vertex.h"
#include "MeshInstance.h"

using namespace std;

class Shader;

class Mesh {
public:
	Mesh(vector<Vertex> vertices);
	Mesh(vector<Vertex> vertices, vector<GLuint> indices);
	Mesh(const string& filePath);
	unsigned int getId() const { return this->meshId; }
	MeshInstance* newInstance(unsigned int modelIndex, Material material);
	void removeModel(unsigned int modelIndex);
	void draw(Shader shader);
protected:
	bool isIndexed;
	GLuint VAO, VBO, EBO, IBO;
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<MeshInstance> instances;
	void init();
	static unsigned int idCounter;
	unsigned int meshId;
};

#endif
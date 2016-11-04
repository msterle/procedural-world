#ifndef MESH_H
#define MESH_H

#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Vertex.h"

using namespace std;

class Shader;

class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	GLuint VAO, VBO, EBO;
	Mesh(vector<Vertex> vertices, vector<GLuint> indices);
	void draw(Shader shader);
};

#endif
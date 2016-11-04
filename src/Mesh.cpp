#include <cstddef>
#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"

#include "Mesh.h"

using namespace std;


// Constructors

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices) {
	this->vertices = vertices;
	this->indices = indices;

	// Generate VAO, VBO, EBO
	glBindVertexArray(this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Bind buffers
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);

	// set vertex attribute pointers
	// position
	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// normal
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	glBindVertexArray(0);
}

// Public methods

void Mesh::draw(Shader shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
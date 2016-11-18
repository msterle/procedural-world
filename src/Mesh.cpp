//#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"

#include "Mesh.h"

// debugging only
#include <iostream>

using namespace std;


//// Constructors

Mesh::Mesh(vector<Vertex> vertices, GLenum drawMode) {
	this->vertices = vertices;
	this->drawMode = drawMode;
	isIndexed = false;
	init();
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, GLenum drawMode) {
	this->vertices = vertices;
	this->indices = indices;
	this->drawMode = drawMode;
	isIndexed = true;
	init();
}

Mesh::Mesh(string filePath) {
	std::vector<unsigned int> vertexPositionIndices, vertexNormalIndices;
	std::vector<glm::vec3> vertexPositions, vertexNormals;

	FILE * file = fopen(filePath.c_str(), "r");

	if(file == NULL){
		cerr << "Couldn't open file " << filePath << endl;
		terminate();
	}

	while(true){
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if(strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertexPosition;
			fscanf(file, "%f %f %f\n", &vertexPosition.x, &vertexPosition.y, &vertexPosition.z );
			vertexPositions.push_back(vertexPosition);
		}
		else if(strcmp(lineHeader, "vn") == 0) {
		    glm::vec3 vertexNormal;
		    fscanf(file, "%f %f %f\n", &vertexNormal.x, &vertexNormal.y, &vertexNormal.z );
		    vertexNormals.push_back(glm::normalize(vertexNormal));
		}
		else if(strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], normalIndex[3];

			//int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

			if (matches == 0){
				cerr << "File can't be read by our simple parser :-( Try exporting with other options" << endl;
				terminate();
			}
			vertexPositionIndices.push_back(vertexIndex[0]);
			vertexPositionIndices.push_back(vertexIndex[1]);
			vertexPositionIndices.push_back(vertexIndex[2]);
			vertexNormalIndices.push_back(normalIndex[0]);
			vertexNormalIndices.push_back(normalIndex[1]);
			vertexNormalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for(unsigned int i = 0; i < vertexPositionIndices.size(); i++) {
		// Get the attributes thanks to the index
		Vertex vertex = {
			vertexPositions[vertexPositionIndices[i] - 1], 
			vertexNormals[vertexNormalIndices[i] - 1]
		};
		// add to our vector
		vertices.push_back(vertex);
	}

	isIndexed = false;
	drawMode = GL_TRIANGLES;
	init();
}


//// Public methods



MeshInstancePtr Mesh::newInstance() {
	return newInstance(glm::mat4(1), Materials::pewter);
}

MeshInstancePtr Mesh::newInstance(glm::mat4 instanceMat) {
	return newInstance(instanceMat, Materials::pewter);
}

MeshInstancePtr Mesh::newInstance(Material material) {
	return newInstance(glm::mat4(1), material);
}
MeshInstancePtr Mesh::newInstance(glm::mat4 instanceMat, Material material) {
	MeshInstancePtr ptr(&instances, instances.size());
	instances.push_back(MeshInstance(ptr, instanceMat, material));
	return ptr;
}

// copy factory method
MeshInstancePtr Mesh::newInstance(MeshInstancePtr orig) {
	MeshInstancePtr ptr(&instances, instances.size());
	instances.push_back(MeshInstance(ptr, orig->instanceMat, orig->material));
	return ptr;
}

void Mesh::draw(Shader* shader, glm::mat4 modelMat) {
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(MeshInstance), &instances.front(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	if(isIndexed) {
		glDrawElementsInstanced(drawMode, indices.size(), GL_UNSIGNED_INT, 0, instances.size());
	}
	else {
		glDrawArraysInstanced(drawMode, 0, vertices.size(), instances.size());
	}
	
	glBindVertexArray(0);
}


//// Private methods

void Mesh::init() {
	// Generate VAO, VBO, EBO, IBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	if(isIndexed)
		glGenBuffers(1, &EBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.front(), GL_STATIC_DRAW);

	// Bind buffers
	glBindVertexArray(VAO);
	if(isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices.front(), GL_STATIC_DRAW);
	}

	// set vertex attribute pointers
	
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	
	// normal
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// instance matrix
	GLsizei vec4Size = sizeof(glm::vec4);
	GLsizei matOffset = offsetof(MeshInstance, instanceMat);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)matOffset);
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(matOffset + vec4Size));
    glVertexAttribDivisor(5, 1);
    
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(matOffset + 2 * vec4Size));
    glVertexAttribDivisor(6, 1);
    
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(matOffset + 3 * vec4Size));
    glVertexAttribDivisor(7, 1);

    // instance material
	GLsizei materialOffset = offsetof(MeshInstance, material);
    
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)materialOffset);
    glVertexAttribDivisor(8, 1);
    
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(materialOffset + vec4Size));
    glVertexAttribDivisor(9, 1);
    
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(materialOffset + 2 * vec4Size));
    glVertexAttribDivisor(10, 1);
    
    glEnableVertexAttribArray(11);
    glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(materialOffset + 3 * vec4Size));
    glVertexAttribDivisor(11, 1);

	glBindVertexArray(0);
}


//// MeshInstancePtr public methods

MeshInstance MeshInstancePtr::operator*() {
	return (*container)[offset];
}

MeshInstance* MeshInstancePtr::operator->() {
	return &(*container)[offset];
}
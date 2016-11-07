//#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "MeshInstance.h"

#include "Mesh.h"

// debugging only
#include <iostream>

using namespace std;


//// Static member initializations

unsigned int Mesh::idCounter = 0;


//// Constructors

Mesh::Mesh(vector<Vertex> vertices) {
	this->isIndexed = false;
	this->vertices = vertices;
	this->init();
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices) {
	this->isIndexed = true;
	this->vertices = vertices;
	this->indices = indices;
	this->init();
}

Mesh::Mesh(const string& filePath) {
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
		this->vertices.push_back(vertex);
	}

	this->isIndexed = false;
	this->init();
}


//// Public methods

MeshInstance* Mesh::newInstance(unsigned int modelIndex, Material material) {
	MeshInstance instance(modelIndex, this->meshId, material);
	this->instances.push_back(instance);
	return &(this->instances.back());
}

// TODO: optimize into one iteration?
void Mesh::removeModel(unsigned int deleteModelIndex) {
	this->instances.erase(remove_if(this->instances.begin(), this->instances.end(), 
		[&deleteModelIndex](const MeshInstance& instance) {
			return instance.modelIndex == deleteModelIndex;
		}));
	for(std::vector<MeshInstance>::iterator itInstance = this->instances.begin(); itInstance != this->instances.end(); ++itInstance) {
		if(itInstance->modelIndex >= deleteModelIndex)
			itInstance->modelIndex--;
	}
}

void Mesh::draw(Shader shader) {
	
	GLuint shaderProgram = shader.getProgramRef();
	GLint loc_modelMat = glGetUniformLocation(shaderProgram, "modelMat");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	glBindBuffer(GL_ARRAY_BUFFER, this->IBO);
	glBufferData(GL_ARRAY_BUFFER, this->instances.size() * sizeof(MeshInstance), &this->instances.front(), GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);

	if(this->isIndexed) {
		glDrawElementsInstanced(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0, this->instances.size());
	}
	else {
		glDrawArraysInstanced(GL_TRIANGLES, 0, this->vertices.size(), this->instances.size());
	}
	
	glBindVertexArray(0);
}


//// Private methods

void Mesh::init() {
	// Generate VAO, VBO, EBO, IBO
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	if(this->isIndexed)
		glGenBuffers(1, &this->EBO);
	glGenBuffers(1, &this->IBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices.front(), GL_STATIC_DRAW);

	// Bind buffers
	glBindVertexArray(this->VAO);
	if(this->isIndexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);
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
	glBindBuffer(GL_ARRAY_BUFFER, this->IBO);
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)0);
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(vec4Size));
    glVertexAttribDivisor(5, 1);
    
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(2 * vec4Size));
    glVertexAttribDivisor(6, 1);
    
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(MeshInstance), (GLvoid*)(3 * vec4Size));
    glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);

	this->meshId = Mesh::idCounter++;
}
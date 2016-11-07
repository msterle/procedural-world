#include "Tree.h"

#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

#include "../include/glm/gtc/type_ptr.hpp"

#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

using namespace std;

// Constructor

Tree::Tree() {
	string objPath = string(PROJECT_ROOT) + string("/res/objects/cylinder16.obj");
	this->loadOBJ(objPath.c_str());
	this->material = &Materials::copper;
	this->modelMat = glm::mat4();
}

// Public methods

void Tree::draw(Shader shader) {
	GLuint shaderProgram = shader.getProgramRef();
	GLint loc_modelMat = glGetUniformLocation(shaderProgram, "modelMat");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(this->modelMat));
	this->mesh->draw(shader);
}







bool Tree::loadOBJ(const char* path)
{
	std::cout << "Loading OBJ file " << path << "..." << std::endl;

	std::vector<unsigned int> vertexIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices, temp_normals;

	FILE * file = fopen(path, "r");

	if( file == NULL ){
		std::cout << "Impossible to open the file ! Are you in the right path ?" << std::endl;
		terminate();
	}

	// initialize min and max x,y,z values
	glm::vec3 max = glm::vec3(-1e99, -1e99, -1e99);
	glm::vec3 min = glm::vec3(1e99, 1e99, 1e99);

	while(true){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if(strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			if(vertex.x < min.x) min.x = vertex.x;
			if(vertex.x > max.x) max.x = vertex.x;
			if(vertex.y < min.y) min.y = vertex.y;
			if(vertex.y > max.y) max.y = vertex.y;
			if(vertex.z < min.z) min.z = vertex.z;
			if(vertex.z > max.z) max.z = vertex.z;
			temp_vertices.push_back(vertex);
		}
		else if(strcmp(lineHeader, "vn") == 0) {
		    glm::vec3 normal;
		    fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
		    //glm::normalize(normal);
		    temp_normals.push_back(normal);
		}
		else if(strcmp(lineHeader, "f") == 0) {
			unsigned int vertexIndex[3], normalIndex[3];

			//int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);

			if (matches == 0){
				std::cout << "File can't be read by our simple parser :-( Try exporting with other options" << std::endl;
				terminate();
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// Center model on origin and scale uniformly to fit (-1,-1,-1) to (1, 1, 1)
	glm::vec3 center = (max + min) / 2.0f;
	float maxDimension = std::max(std::max(max.x - min.x, max.y - min.y), max.z - min.z);
	for(int i = 0; i < temp_vertices.size(); i++) {
		temp_vertices[i] -= center;
		temp_vertices[i] *= 2.0f / maxDimension;
	}


	vector<Vertex> vertices;

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		// Get the attributes thanks to the index
		Vertex vertex = {
			temp_vertices[ vertexIndices[i]-1 ], 
			temp_normals[ normalIndices[i]-1 ]
		};
		
		// Put the vertex in the std::vector
		vertices.push_back(vertex);
	}

	this->mesh = new Mesh(vertices);

	return true;
}
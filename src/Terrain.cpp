#include "Terrain.h"

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/CImg.h"
#include <vector>

#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "Vertex.h"
#include "Model.h"
#include "Material.h"

// debugging only!
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// Public methods

Terrain::Terrain() {
	//glGenBuffers(1, &this->colorBuffer);

}

/*
void Terrain::buildFromHeightmap(string heightmapPath, string colorPath, int scale) {
	this->heightImg = cimg_library::CImg<unsigned char>(heightmapPath.c_str());
	this->colorImg = cimg_library::CImg<unsigned char>(colorPath.c_str());
	// input check
	if( heightImg.width() != this->colorImg.width() || heightImg.height() != this->colorImg.height() ) {
		cerr << "Error: input images not equal dimensions";
		exit(-1);
	}
	// build vertices
	this->vertices.clear();
	this->vertexColors.clear();
	int sourceWidth = this->heightImg.width(), sourceHeight = this->heightImg.width();
	int width = scale * sourceWidth;
	int height = scale * sourceHeight;
	float yMax = 0;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			// For each pixel add a vertex with pixel's x and y values, z determined
			// from heightmap image's first channel value and color copied from color image
			glm::vec3 vertex;
			vertex.x = (float) x / (width - 1) - 0.5f;
			vertex.z = (float) y / (height - 1) - 0.5f;
			vertex.y = upsample(x, y, 0, this->heightImg, sourceWidth, sourceHeight, scale) / 255;
			if(vertex.y > yMax)
				yMax = vertex.y;
			this->vertices.push_back(vertex);
			
			glm::vec3 vertexColor;
			vertexColor.r = upsample(x, y, 0, this->colorImg, sourceWidth, sourceHeight, scale) / 255;
			vertexColor.g = upsample(x, y, 1, this->colorImg, sourceWidth, sourceHeight, scale) / 255;
			vertexColor.b = upsample(x, y, 2, this->colorImg, sourceWidth, sourceHeight, scale) / 255;
			this->vertexColors.push_back(vertexColor);
		}
	}

	// Center object on Y plane according to maximum y value
	for(std::vector<glm::vec3>::iterator it = this->vertices.begin(); it != this->vertices.end(); ++it) {
   		it->y -= yMax / 2;
	}

	this->normals = this->getNormals(width, width);

	// build indices
	this->indices.clear();
	for(int y = 0; y < height - 1; y++) {
		for(int x = 0; x < width; x++) {
			this->indices.push_back(x + y * width);
			this->indices.push_back(x + (y + 1) * width);
		}
		// Restart primitive at end of row
		if(y < height - 2) {
			this->indices.push_back(65535);
		}
	}

	this->scale(glm::vec3(1, 0.5, 1));

	// bind buffers
	glBindVertexArray(this->VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	// bind normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// bind color buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertexColors.size() * sizeof(glm::vec3), &this->vertexColors.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	GLuint shaderProgram = this->shader.getProgramRef();

	GLuint useLightingLoc = glGetUniformLocation(shaderProgram, "useLighting");

	glUniform1i(useLightingLoc, 0);
}

void Terrain::generateHills(int width, int number) {
	float minRadius = (float)width / 20;
	float maxRadius = (float)width / 5;

	this->vertices.clear();
	this->vertexColors.clear();
	this->vertices.resize(pow(width, 2));
	float height, minHeight = 0, maxHeight = 0;
	srand(time(NULL));
	for(int i = 0; i < number; i++) {
		float r = minRadius + (float)rand() / (RAND_MAX / (maxRadius - minRadius));
		float cx = rand() % width, cy = rand() % width;
		for(int y = 0; y < width; y++) {
			for(int x = 0; x < width; x++) {
				height = this->vertices[x + y * width].y + max(0.0f, (float)(pow(r, 2) - (pow(x - cx, 2) + pow(y - cy, 2))));
				minHeight = min(minHeight, height);
				maxHeight = max(maxHeight, height);
				this->vertices[x + y * width] = glm::vec3(x, height, y);
			}
		}
	}

	this->normals = this->getNormals(width, width);

	this->indices.clear();
	for(int y = 0; y < width - 1; y++) {
		for(int x = 0; x < width; x++) {
			this->indices.push_back(x + y * width);
			this->indices.push_back(x + (y + 1) * width);
		}
		// Restart primitive at end of row
		if(y < width - 2) {
			this->indices.push_back(65535);
		}
	}

	this->translate(glm::vec3(-width / 2, -(maxHeight + minHeight) / 2, -width / 2));
	this->scale(glm::vec3(1.0f / width, maxHeight == minHeight ? 1 : 1.0f / (maxHeight - minHeight), 1.0f / width));
	this->scale(glm::vec3(1, 0.25, 1));

	this->material = &Materials::copper;

	// bind buffers
	glBindVertexArray(this->VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	// bind normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	GLuint shaderProgram = this->shader.getProgramRef();

	GLuint MaterialAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	GLuint MaterialDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLuint MaterialSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLuint MaterialShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

	glUniform4fv(MaterialAmbientLoc, 1, glm::value_ptr(this->material->ambient));
	glUniform4fv(MaterialDiffuseLoc, 1, glm::value_ptr(this->material->diffuse));
	glUniform4fv(MaterialSpecularLoc, 1, glm::value_ptr(this->material->specular));
	glUniform1f(MaterialShininessLoc, this->material->shininess);

	GLuint useLightingLoc = glGetUniformLocation(shaderProgram, "useLighting");

	glUniform1i(useLightingLoc, 1);
}
*/


void Terrain::generateDiamondSquare(int aproxWidth, float roughness) {
	
	width = pow(2, round(log(aproxWidth - 1) / log(2))) + 1;
	length = width;
	vector<Vertex> vertices(pow(width, 2));
	
	// set initial corner values
	vertices[0].position = glm::vec3(0, (float)rand() / RAND_MAX * (width - 1) * roughness, 0);
	vertices[width - 1].position = glm::vec3(width - 1, (float)rand() / RAND_MAX * (width - 1) * roughness, 0);
	vertices[width * (width - 1)].position = glm::vec3(0, (float)rand() / RAND_MAX * (width - 1) * roughness, width - 1);
	vertices[width * width - 1].position = glm::vec3(width - 1, (float)rand() / RAND_MAX * (width - 1) * roughness, width - 1);
	
	// enter recursive generation algorithm
	recurseDiamondSquare(&vertices, width, width - 1, roughness);

	// calculate indices
	vector<GLuint> indices;
	for(int y = 0; y < width - 1; y++) {
		for(int x = 0; x < width; x++) {
			indices.push_back(x + y * width);
			indices.push_back(x + (y + 1) * width);
		}
		// Restart primitive at end of row
		if(y < width - 2) {
			indices.push_back(65535);
		}
	}

	// calculate vertex normals
	generateNormals(&vertices, width, width);

	// create mesh and mesh instance
	mesh = newMesh(vertices, indices, GL_TRIANGLE_STRIP);
	instancePtr = mesh->newInstance(Materials::copper);

	// center and scale mesh instance to fit (-1, -1, -1) (1, 1, 1) cube
	float minHeight = 1, maxHeight = 0;
	for(int y = 0; y < width; y++) {
		for(int x = 0; x < width; x++) {
			minHeight = min(minHeight, vertices[x + y * width].position.y);
			maxHeight = max(maxHeight, vertices[x + y * width].position.y);
		}
	}
	instancePtr->translate(glm::vec3(-width / 2, 0, -width / 2));
	instancePtr->scale(glm::vec3(2.0f / width, 2.0f / width, 2.0f / width));

	// scale model to width
	scale(glm::vec3(width / 2, width / 2, width / 2));
}

void Terrain::generatePlane(float width, float length) {
	vector<Vertex> vertices;
	float x = width / 2, z = length / 2;
	vertices.push_back(Vertex { glm::vec3(-x, 0, -z), glm::vec3(0, 1, 0) });
	vertices.push_back(Vertex { glm::vec3(x, 0, -z), glm::vec3(0, 1, 0) });
	vertices.push_back(Vertex { glm::vec3(-x, 0, z), glm::vec3(0, 1, 0) });
	vertices.push_back(Vertex { glm::vec3(x, 0, z), glm::vec3(0, 1, 0) });

	vector<GLuint> indices = { 0, 2, 1, 3 };

	Mesh* mesh = newMesh(vertices, indices, GL_TRIANGLE_STRIP);
	MeshInstancePtr ptr = mesh->newInstance(Materials::copper);
}

float Terrain::getYAtXZWorld(float x, float z) {
	glm::vec4 position(x, 0, z, 1);

	// convert to instance coordinates
	position = glm::inverse(instancePtr->instanceMat) * glm::inverse(modelMat) * position;

	// get integer and fractrional parts, clamp between 0 and width/length
	float fpartX, fpartZ, temp;
	int prevX, nextX, prevZ, nextZ;
	
	fpartX = modf(position.x, &temp);
	prevX = min(max(temp, 0.0f), width);
	nextX = min(prevX + 1, (int)width);

	fpartZ = modf(position.z, &temp);
	prevZ = min(max(temp, 0.0f), width);
	nextZ = min(prevZ + 1, (int)width);

	vector<Vertex> vertices = mesh->getVertices();

	// interpolate Y from surrounding 4 vertices
	position.y = vertices[prevX + prevZ * width].position.y * fpartX * fpartZ
		+ vertices[nextX + prevZ * width].position.y * (1.0f - fpartX) * fpartZ
		+ vertices[prevX + nextZ * width].position.y * fpartX * (1.0f - fpartZ)
		+ vertices[nextX + nextZ * width].position.y * (1.0f - fpartX) * (1.0f - fpartZ);

	// convert back to world coordinates
	position = modelMat * instancePtr->instanceMat * position;

	return position.y;
}


/*
	for(int y = 0; y < width; y++) {
		for(int x = 0; x < width; x++) {
			vertices[x + y * width] = Vertex { glm::vec3(x, 0, y), glm::vec3(0, 1, 0) };
		}
	}

	vector<GLuint> indices;
	for(int y = 0; y < width - 1; y++) {
		for(int x = 0; x < width; x++) {
			indices.push_back(x + y * width);
			indices.push_back(x + (y + 1) * width);
		}
		// Restart primitive at end of row
		if(y < width - 2) {
			indices.push_back(65535);
		}
	}

	translate(glm::vec3(-width / 2, 0, -width / 2));

	this->material = &Materials::pewter;

	// bind buffers
	glBindVertexArray(this->VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	// bind normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals.front(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	GLuint shaderProgram = this->shader.getProgramRef();

	GLuint MaterialAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
	GLuint MaterialDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
	GLuint MaterialSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
	GLuint MaterialShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");

	glUniform4fv(MaterialAmbientLoc, 1, glm::value_ptr(this->material->ambient));
	glUniform4fv(MaterialDiffuseLoc, 1, glm::value_ptr(this->material->diffuse));
	glUniform4fv(MaterialSpecularLoc, 1, glm::value_ptr(this->material->specular));
	glUniform1f(MaterialShininessLoc, this->material->shininess);

	GLuint useLightingLoc = glGetUniformLocation(shaderProgram, "useLighting");

	glUniform1i(useLightingLoc, 1);
}
*/



// Protected methods

void Terrain::recurseDiamondSquare(vector<Vertex>* vertices, int width, int size, float roughness) {
	// check for end of recursion
	if(size < 2)
		return;
	int half = size / 2;
	srand(time(NULL));
	// do diamond
	for(int y = half; y < width; y += size) {
		for(int x = half; x < width; x += size) {
			(*vertices)[x + y * width].position.x = x;
			(*vertices)[x + y * width].position.z = y;
			(*vertices)[x + y * width].position.y = 
				((*vertices)[x - half + (y - half) * width].position.y
				+ (*vertices)[x + half + (y - half) * width].position.y
				+ (*vertices)[x - half + (y + half) * width].position.y
				+ (*vertices)[x + half + (y + half) * width].position.y) / 4
			    + ((float)rand() / RAND_MAX - 0.5) * pow(size, 0.5) * roughness;
		}
	}
	// do square
	for(int y = 0; y < width; y += half) {
		for(int x = (y + half) % size; x < width; x += size) {
			int n = 0;
			float height = 0;
			if(y > 0) {
				height += (*vertices)[x + (y - half) * width].position.y;
				n++;
			}
			if(x > 0) {
				height += (*vertices)[x - half + y * width].position.y;
				n++;
			}
			if(y < width - 1) {
				height += (*vertices)[x + (y + half) * width].position.y;
				n++;
			}
			if(x < width - 1) {
				height += (*vertices)[x + half + y * width].position.y;
				n++;
			}
			(*vertices)[x + y * width].position.x = x;
			(*vertices)[x + y * width].position.z = y;
			(*vertices)[x + y * width].position.y = height / n + ((float)rand() 
				/ RAND_MAX - 0.5) * size * roughness;
		}
	}
	// recurse
	recurseDiamondSquare(vertices, width, half, roughness);
}

// generate normals based on surrounding faces
void Terrain::generateNormals(vector<Vertex>* vertices, int width, int length) {
	for(int y = 0; y < length; y++) {
		for(int x = 0; x < width; x++) {
			glm::vec3 normal;
			if(x > 0 && y > 0) {
				// include normal from top-left face
				normal += glm::normalize(glm::cross(
					(*vertices)[x + (y - 1) * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x - 1 + y * width].position - (*vertices)[x + y * width].position));
			}
			if(x < width - 1 && y > 0) {
				// include normals from top-right faces
				normal += glm::normalize(glm::cross(
					(*vertices)[x + 1 + (y - 1) * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x + (y - 1) * width].position - (*vertices)[x + y * width].position));
				normal += glm::normalize(glm::cross(
					(*vertices)[x + 1 + y * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x + 1 + (y - 1) * width].position - (*vertices)[x + y * width].position));
			}
			if(x < width - 1 && y < length - 1) {
				// include normal from bottom-right face
				normal += glm::normalize(glm::cross(
					(*vertices)[x + (y + 1) * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x + 1 + y * width].position - (*vertices)[x + y * width].position));
			}
			if(x > 0 && y < length - 1) {
				// include normals from bottom-left faces
				normal += glm::normalize(glm::cross(
					(*vertices)[x - 1 + (y + 1) * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x + (y + 1) * width].position - (*vertices)[x + y * width].position));
				normal += glm::normalize(glm::cross(
					(*vertices)[x - 1 + y * width].position - (*vertices)[x + y * width].position, 
					(*vertices)[x - 1 + (y + 1) * width].position - (*vertices)[x + y * width].position));
			}
			// set normal to normalized sum of surrounding face normals
			(*vertices)[x + y * width].normal = glm::normalize(normal);
		}
	}
}
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/CImg.h"
#include <vector>

#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "Model.h"
#include "Terrain.h"
#include "Material.h"

// debugging only!
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// Public methods

Terrain::Terrain() {
	glGenBuffers(1, &this->colorBuffer);
}

void Terrain::buildFromHeightmap(string heightmapPath, string colorPath, int scale) {
	this->heightImg = cimg_library::CImg<unsigned char>(heightmapPath.c_str());
	this->colorImg = cimg_library::CImg<unsigned char>(colorPath.c_str());
	// input check
	if( heightImg.width() != this->colorImg.width() || heightImg.height() != this->colorImg.height() ) {
		cerr << "Error: input images not equal dimensions";
		exit(-1);
	}
	this->resampleHeightmap(scale);	
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



void Terrain::generateDiamondSquare(int aproxWidth, float roughness) {
	int width = pow(2, round(log(aproxWidth - 1) / log(2))) + 1;
	this->vertices.clear();
	this->vertexColors.clear();
	this->vertices.resize(pow(width, 2));
	this->vertices[0] = glm::vec3(0, ((float)rand() / RAND_MAX - 0.5) * (width - 1) * roughness, 0);
	this->vertices[width - 1] = glm::vec3(width - 1, ((float)rand() / RAND_MAX - 0.5) * (width - 1) * roughness, 0);
	this->vertices[width * (width - 1)] = glm::vec3(0, ((float)rand() / RAND_MAX - 0.5) * (width - 1) * roughness, width - 1);
	this->vertices[width * width - 1] = glm::vec3(width - 1, ((float)rand() / RAND_MAX - 0.5) * (width - 1) * roughness, width - 1);
	this->subDiamondSquare(width, width - 1, roughness);

	float minHeight = 1, maxHeight = 0;
	for(int y = 0; y < width; y++) {
		for(int x = 0; x < width; x++) {
			minHeight = min(minHeight, this->vertices[x + y * width].y);
			maxHeight = max(maxHeight, this->vertices[x + y * width].y);
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

	this->scale(glm::vec3(128, 128, 128));

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

void Terrain::generatePlane(int width) {
	this->vertices.clear();
	this->vertexColors.clear();
	this->vertices.resize(pow(width, 2));
	for(int y = 0; y < width; y++) {
		for(int x = 0; x < width; x++) {
			this->vertices[x + y * width] = glm::vec3(x, 0, y);
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

	this->translate(glm::vec3(-width / 2, 0, -width / 2));

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

void Terrain::resampleHeightmap(int scale) {
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

void Terrain::draw() {
	/*
	GLint loc_modelMat = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(this->modelMat));
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLE_STRIP, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	*/
	GLuint shaderProgram = this->shader.getProgramRef();
	GLint loc_modelMat = glGetUniformLocation(shaderProgram, "modelMat");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(this->modelMat));
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLE_STRIP, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


// Protected methods

float Terrain::upsample(int x, int y, int c, const cimg_library::CImg<unsigned char>& image, int width, int height, int scale) {
	// Weighted average of four surrounding source pixels
	// note: requires image width & height to reduce overhead of calling CImg methods
	// ex: in 4x4 upsampling (scale = 4), output pixel (1, 2) has weighted average of
	// surrounding 4 input pixels A (0,0), B (1, 0), C(0, 1) and D (1, 1)
	// 6/16 * A + 2/16 * B + 6/16 * C + 2/16 * D
	int xmod = x % scale, ymod = y % scale;
	int xLast = x / scale + 1 < width ? 1 : 0;
	int yLast = y / scale + 1 < height ? 1 : 0;
	// Optimized from operator() to
	return (  (float) *image.data(x / scale,         y / scale,         0, c) * (scale - xmod) * (scale - ymod) 
			+ (float) *image.data(x / scale + xLast, y / scale,         0, c) * xmod *           (scale - ymod) 
			+ (float) *image.data(x / scale,         y / scale + yLast, 0, c) * (scale - xmod) * ymod 
			+ (float) *image.data(x / scale + xLast, y / scale + yLast, 0, c) * xmod *           ymod ) 
		/ (scale * scale);     // faster than pow(scale, 2)
}

void Terrain::subDiamondSquare(int width, int size, float roughness) {
	if(size < 2)
		return;
	int half = size / 2;
	srand(time(NULL));
	for(int y = half; y < width; y += size) {
		for(int x = half; x < width; x += size) {
			this->vertices[x + y * width] = glm::vec3();
			this->vertices[x + y * width].x = x;
			this->vertices[x + y * width].z = y;
			this->vertices[x + y * width].y = (this->vertices[x - half + (y - half) * width].y
				                            + this->vertices[x + half + (y - half) * width].y
				                            + this->vertices[x - half + (y + half) * width].y
				                            + this->vertices[x + half + (y + half) * width].y) / 4
			                                + ((float)rand() / RAND_MAX - 0.5) * pow(size, 0.5) * roughness;
		}
	}
	for(int y = 0; y < width; y += half) {
		for(int x = (y + half) % size; x < width; x += size) {
			int n = 0;
			float height = 0;
			if(y > 0) {
				height += this->vertices[x + (y - half) * width].y;
				n++;
			}
			if(x > 0) {
				height += this->vertices[x - half + y * width].y;
				n++;
			}
			if(y < width - 1) {
				height += this->vertices[x + (y + half) * width].y;
				n++;
			}
			if(x < width - 1) {
				height += this->vertices[x + half + y * width].y;
				n++;
			}
			this->vertices[x + y * width] = glm::vec3();
			this->vertices[x + y * width].x = x;
			this->vertices[x + y * width].z = y;
			this->vertices[x + y * width].y = height / n + ((float)rand() / RAND_MAX - 0.5) * size * roughness;
		}
	}
	this->subDiamondSquare(width, half, roughness);
}

vector<glm::vec3> Terrain::getNormals(int width, int height) {
	vector<glm::vec3> normals(width * height);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			glm::vec3 normal;
			if(x > 0 && y > 0) {
				normal += glm::normalize(glm::cross(this->vertices[x + (y - 1) * width] - this->vertices[x + y * width], this->vertices[x - 1 + y * width] - this->vertices[x + y * width]));
			}
			if(x < width - 1 && y > 0) {
				normal += glm::normalize(glm::cross(this->vertices[x + 1 + (y - 1) * width] - this->vertices[x + y * width], this->vertices[x + (y - 1) * width] - this->vertices[x + y * width]));
				normal += glm::normalize(glm::cross(this->vertices[x + 1 + y * width] - this->vertices[x + y * width], this->vertices[x + 1 + (y - 1) * width] - this->vertices[x + y * width]));
			}
			if(x < width - 1 && y < height - 1) {
				normal += glm::normalize(glm::cross(this->vertices[x + (y + 1) * width] - this->vertices[x + y * width], this->vertices[x + 1 + y * width] - this->vertices[x + y * width]));
			}
			if(x > 0 && y < height - 1) {
				normal += glm::normalize(glm::cross(this->vertices[x - 1 + (y + 1) * width] - this->vertices[x + y * width], this->vertices[x + (y + 1) * width] - this->vertices[x + y * width]));
				normal += glm::normalize(glm::cross(this->vertices[x - 1 + y * width] - this->vertices[x + y * width], this->vertices[x - 1 + (y + 1) * width] - this->vertices[x + y * width]));
			}
			normals[x + y * width] = glm::normalize(normal);
		}
	}
	return normals;
}
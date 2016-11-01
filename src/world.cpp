#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "terrain.h"
#include "camera.h"

#include "world.h"

// debugging only
#include <iostream>

using namespace std;

// Public Methods

World::World() {
	// Set up shaders
	string vertexShaderPath = string(PROJECT_ROOT) + string("/src/shaders/lighting.vert");
	string fragmentShaderPath = string(PROJECT_ROOT) + string("/src/shaders/lighting.frag");
	this->shaderProgram = ShaderProgram(vertexShaderPath, fragmentShaderPath);
	this->uni_viewMat = glGetUniformLocation(shaderProgram.getProgramRef(), "viewMat");
	this->uni_projMat = glGetUniformLocation(shaderProgram.getProgramRef(), "projMat");

	// Set up terrain
	/*
	string terrainHeightmapPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_HEIGHTMAP);
	string terrainColorPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_COLOR);
	this->terrain.setShaderProgram(shaderProgram.getProgramRef());
	this->terrain.buildFromHeightmap(terrainHeightmapPath, terrainColorPath);
	*/
	
	/*
	this->terrain.setShaderProgram(shaderProgram.getProgramRef());
	this->terrain.generateHills(200, 200);
	*/


	this->terrain.setShaderProgram(shaderProgram.getProgramRef());
	this->terrain.generateDiamondSquare(128, 0.5f);
	

	// Set up camera
	this->camera.setPosition(glm::vec3(1, 1, 1));
	this->camera.lookAt(glm::vec3(0, 0, 0));
	int width, height;
}

void World::draw() {
	// Clear the colorbuffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind transformation matrices to shader program
	glUniformMatrix4fv(this->uni_viewMat, 1, GL_FALSE, glm::value_ptr(this->camera.getViewMat()));
	glUniformMatrix4fv(this->uni_projMat, 1, GL_FALSE, glm::value_ptr(this->camera.getProjMat()));

	GLuint LightPosCLoc = glGetUniformLocation(shaderProgram.getProgramRef(), "lightPositionC");
	GLuint LightColorLoc = glGetUniformLocation(shaderProgram.getProgramRef(), "lightColor");

	glUniform4fv(LightPosCLoc, 1, glm::value_ptr(this->camera.getViewMat() * glm::vec4(5.0, 10.0, 20.0, 0)));
	glUniform4fv(LightColorLoc, 1, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));
	
	// Draw models
	this->terrain.draw();
}
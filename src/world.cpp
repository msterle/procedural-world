#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "terrain.h"
#include "camera.h"

#include "world.h"

using namespace std;

// Public Methods

World::World() {
	// Set up shaders
	string vertexShaderPath = string(PROJECT_ROOT) + string("/src/shaders/vertex.shader");
	string fragmentShaderPath = string(PROJECT_ROOT) + string("/src/shaders/fragment.shader");
	this->shaderProgram = ShaderProgram(vertexShaderPath, fragmentShaderPath);
	this->uni_viewMat = glGetUniformLocation(shaderProgram.getProgramRef(), "view");
	this->uni_projMat = glGetUniformLocation(shaderProgram.getProgramRef(), "proj");

	// Set up terrain
	string terrainHeightmapPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_HEIGHTMAP);
	string terrainColorPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_COLOR);
	this->terrain.setShaderProgram(shaderProgram.getProgramRef());
	this->terrain.buildFromHeightmap(terrainHeightmapPath, terrainColorPath);

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
	
	// Draw models
	this->terrain.draw();
}
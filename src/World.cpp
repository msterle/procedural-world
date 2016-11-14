#include <string>
#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Terrain.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "Tree.h"
#include "ParaTree.h"

#include "World.h"

// debugging only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// Public Methods

World::World() {
	// Set up shaders
	string vertexShaderPath = string(PROJECT_ROOT) + string("/src/shaders/instance.vert");
	string fragmentShaderPath = string(PROJECT_ROOT) + string("/src/shaders/lighting.frag");
	this->shader = Shader(vertexShaderPath, fragmentShaderPath);
	this->uni_viewMat = glGetUniformLocation(shader.getProgramRef(), "viewMat");
	this->uni_projMat = glGetUniformLocation(shader.getProgramRef(), "projMat");

	this->terrain.generateDiamondSquare(128, 128);

	// basic l-system tree
	//Tree* tree = new Tree(2);
	//this->models.push_back(tree);

	// parametric l-system tree
	ParaTree* ptree = new ParaTree(ParaTree::Presets::d2);
	this->models.push_back(ptree);

	// place tree at ground level
	glm::vec3 treePos(0, 0, 0);
	ptree->translate(glm::vec3(
		treePos.x, 
		terrain.getYAtXZWorld(treePos.x, treePos.z), 
		treePos.z));

	// Set up camera
	glm::vec3 cameraPos(8, 2, 15);
	this->camera.setPosition(glm::vec3(
		cameraPos.x, 
		terrain.getYAtXZWorld(cameraPos.x, cameraPos.z) + cameraPos.y, 
		cameraPos.z));
	this->camera.lookAt(glm::vec3(0, this->camera.getPosition().y + 3.5, 0));

	// init constant uniforms
	GLuint shaderProgram = this->shader.getProgramRef();

	GLuint LightPosCLoc = glGetUniformLocation(shaderProgram, "lightPositionC");
	GLuint LightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

	glUniform4fv(LightPosCLoc, 1, glm::value_ptr(this->camera.getViewMat() * glm::vec4(5.0, 10.0, 20.0, 0)));
	glUniform4fv(LightColorLoc, 1, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));

	GLuint useLightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
	glUniform1i(useLightingLoc, 1);
}

void World::draw() {
	// Clear the colorbuffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind transformation matrices to shader program
	glUniformMatrix4fv(this->uni_viewMat, 1, GL_FALSE, glm::value_ptr(this->camera.getViewMat()));
	glUniformMatrix4fv(this->uni_projMat, 1, GL_FALSE, glm::value_ptr(this->camera.getProjMat()));
	
	// Draw models
	this->terrain.draw(shader);
	for(list<Model*>::iterator it = this->models.begin(); it != this->models.end(); it++) {
		(*it)->draw(shader);
	}
}
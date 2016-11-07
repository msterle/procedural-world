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

	/*
	this->terrain.setShader(shader);
	this->terrain.generateDiamondSquare(128, 0.5f);
	*/

	this->terrain.generatePlane(128, 128);
	
	Model* model = new Model();
	models.push_back(model);

	string objPath = string(PROJECT_ROOT) + string("/res/objects/cylinder16.obj");
	Mesh* mesh = model->newMesh(objPath);
	//mesh->newInstance(Materials::pewter);
	MeshInstancePtr instancePtr = mesh->newInstance(Materials::gold);
	model->rotate(glm::pi<float>() / 4, glm::vec3(0, 0, 1));
	model->translate(glm::vec3(0, 2, 0));
	instancePtr->scale(glm::vec3(0.5, 2, 0.5));
	mesh->newInstance(instancePtr)->translate(glm::vec3(3, 0, -1));
	
	/*
	// set up objects
	//this->trees.push_back(Tree());
	ModelManager& mm = ModelManager::getInstance();

	string objPath = string(PROJECT_ROOT) + string("/res/objects/cylinder16.obj");
	Mesh* cylinderMesh = mm.newMesh(objPath);


	ModelNew* tubeModel = mm.newModel();

	MeshInstance* cylinderInstance = mm.newMeshInstance(tubeModel->getIndex(), cylinderMesh->getId(), Materials::copper);
	cylinderInstance->translate(glm::vec3(0, 5, -2));
	*/



	// Set up camera
	this->camera.setPosition(glm::vec3(5, 5, 5));
	this->camera.lookAt(glm::vec3(0, 0, 0));
}

void World::draw() {
	// Clear the colorbuffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind transformation matrices to shader program
	glUniformMatrix4fv(this->uni_viewMat, 1, GL_FALSE, glm::value_ptr(this->camera.getViewMat()));
	glUniformMatrix4fv(this->uni_projMat, 1, GL_FALSE, glm::value_ptr(this->camera.getProjMat()));

	GLuint shaderProgram = this->shader.getProgramRef();

	GLuint LightPosCLoc = glGetUniformLocation(shaderProgram, "lightPositionC");
	GLuint LightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");

	glUniform4fv(LightPosCLoc, 1, glm::value_ptr(this->camera.getViewMat() * glm::vec4(5.0, 10.0, 20.0, 0)));
	glUniform4fv(LightColorLoc, 1, glm::value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));

	GLuint useLightingLoc = glGetUniformLocation(shaderProgram, "useLighting");
	glUniform1i(useLightingLoc, 1);
	
	// Draw models
	this->terrain.draw(shader);

	for(list<Model*>::iterator it = this->models.begin(); it != this->models.end(); it++) {
		(*it)->draw(shader);
	}

	/*
	ModelManager& mm = ModelManager::getInstance();
	mm.draw(this->shader);
	*/

	/*
	for(vector<Tree>::iterator it = this->trees.begin(); it != this->trees.end(); ++it) {
		it->draw(this->shader);
	}
	*/
}
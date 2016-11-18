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
#include "helpers.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Filter.h"

#include "World.h"

// debugging only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// constructor

World::World() {
	// set up terrain
	//terrain.generateDiamondSquare(100, 0.1, 0.25);
	terrain.generatePlane(500, 500);

	// set up trees
	ParaTree* ptree = new ParaTree(ParaTree::Presets::d2);
	glm::vec3 treePos(0, 0, 0);
	ptree->translate(glm::vec3(
		treePos.x, 
		terrain.getYAtXZWorld(treePos.x, treePos.z), 
		treePos.z));
	models.push_back(ptree);

	// set up camera
	glm::vec3 cameraPos(8, 2, 15);
	camera.setPosition(glm::vec3(
		cameraPos.x, 
		terrain.getYAtXZWorld(cameraPos.x, cameraPos.z) + cameraPos.y, 
		cameraPos.z));
	camera.lookAt(glm::vec3(0, camera.getPosition().y + 3.5, 0));

	// set up light
	glm::vec3 centerPos(0, terrain.getYAtXZWorld(0, 0), 0);
	light = {glm::vec3(250.0, 1000.0, 500.0), glm::vec4(1.0, 1.0, 1.0, 1.0)};

	//// set up shaders
	// primary shader
	primaryShader = new Shader(PathHelper::shader("primary.vert"), 
		PathHelper::shader("primary.frag"));
	loc_viewPos = glGetUniformLocation(primaryShader->getRef(), "viewPos");
	loc_viewMat = glGetUniformLocation(primaryShader->getRef(), "viewMat");
	loc_projMat = glGetUniformLocation(primaryShader->getRef(), "projMat");
	loc_lightMatPrimary = glGetUniformLocation(primaryShader->getRef(), "lightMat");
	primaryShader->use();
	glUniform4fv(glGetUniformLocation(primaryShader->getRef(), "lightPosition"), 
		1, glm::value_ptr(glm::vec4(light.position, 1)));
	glUniform4fv(glGetUniformLocation(primaryShader->getRef(), "lightColor"), 
		1, glm::value_ptr(light.color));
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "useLighting"), 1);
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "PCFSamples"), 
		params.PCFSamples);

	// shadowmap shader
	shadowShader = new Shader(PathHelper::shader("shadow.vert"), 
		PathHelper::shader("shadow.frag"));
	loc_lightMatShadow = glGetUniformLocation(shadowShader->getRef(), "lightMat");
	
	depthTex = new Texture(GL_RG32F, params.shadowWidth, 
		params.shadowHeight, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER, Texture::Border(1));
	depthFBO = new FrameBuffer(depthTex);

	Filter filter(vector<float>{1, 2, 3, 4, 5, 6, 7, 8, 9}, 2);
}


// destructor

World::~World() {
	delete primaryShader;
	delete shadowShader;
	delete depthFBO;
	delete depthTex;
	for(Model* m : models)
		delete m;
}


// Public Methods

void World::draw(GLFWwindow* window) {
	glm::vec3 cameraPos = camera.getPosition();

	// DEBUG
	/*
	light.lightMat = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -50.0f, 50.0f)
		* glm::lookAt(glm::normalize(light.position) + cameraPos, cameraPos, glm::vec3(0, 1, 0));
	*/
	light.lightMat = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -50.0f, 50.0f)
		* glm::lookAt(glm::normalize(light.position), glm::vec3(0), glm::vec3(0, 1, 0));

	//// Render shadow map
	shadowShader->use();
	glUniformMatrix4fv(loc_lightMatShadow, 1, GL_FALSE, glm::value_ptr(light.lightMat));
	glViewport(0, 0, params.shadowWidth, params.shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO->getRef());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glCullFace(GL_FRONT);

	// Draw models
	terrain.draw(*shadowShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(*shadowShader);
	}

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// DEBUG
	//DebugHelper::renderTex(depthTex->getRef());


	//// Render main
	// Bind world uniforms
	primaryShader->use();
	glUniform4fv(loc_viewPos, 1, glm::value_ptr(glm::vec4(camera.getPosition(), 1.0)));
	glUniformMatrix4fv(loc_viewMat, 1, GL_FALSE, glm::value_ptr(camera.getViewMat()));
	glUniformMatrix4fv(loc_projMat, 1, GL_FALSE, glm::value_ptr(camera.getProjMat()));
	glUniformMatrix4fv(loc_lightMatPrimary, 1, GL_FALSE, glm::value_ptr(light.lightMat));

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTex->getRef());
	
	// Draw models
	terrain.draw(*primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(*primaryShader);
	}
}
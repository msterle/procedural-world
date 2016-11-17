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

#include "World.h"

// debugging only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// Public Methods

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
	primaryShader = Shader(PathHelper::shader("primary.vert"), 
		PathHelper::shader("primary.frag"));
	loc_viewPos = glGetUniformLocation(primaryShader.getProgramRef(), "viewPos");
	loc_viewMat = glGetUniformLocation(primaryShader.getProgramRef(), "viewMat");
	loc_projMat = glGetUniformLocation(primaryShader.getProgramRef(), "projMat");
	loc_lightMatPrimary = glGetUniformLocation(primaryShader.getProgramRef(), "lightMat");
	primaryShader.use();
	glUniform4fv(glGetUniformLocation(primaryShader.getProgramRef(), "lightPosition"), 
		1, glm::value_ptr(glm::vec4(light.position, 1)));
	glUniform4fv(glGetUniformLocation(primaryShader.getProgramRef(), "lightColor"), 
		1, glm::value_ptr(light.color));
	glUniform1i(glGetUniformLocation(primaryShader.getProgramRef(), "useLighting"), 1);
	glUniform1i(glGetUniformLocation(primaryShader.getProgramRef(), "PCFSamples"), 
		params.PCFSamples);

	// shadowmap shader
	shadowShader = Shader(PathHelper::shader("shadow.vert"), 
		PathHelper::shader("shadow.frag"));
	loc_lightMatShadow = glGetUniformLocation(shadowShader.getProgramRef(), "lightMat");
	glGenFramebuffers(1, &shadowDepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
	glGenTextures(1, &shadowDepthTex);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	
	shadowDepthTex = TextureHelper::newTexture2D(GL_RG32F, params.shadowWidth, 
		params.shadowHeight, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER, TextureHelper::Border(1));
	shadowDepthFBO = TextureHelper::newFrameBufferColor(shadowDepthTex);
}

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
	shadowShader.use();
	glUniformMatrix4fv(loc_lightMatShadow, 1, GL_FALSE, glm::value_ptr(light.lightMat));
	glViewport(0, 0, params.shadowWidth, params.shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glCullFace(GL_FRONT);

	// Draw models
	terrain.draw(shadowShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(shadowShader);
	}

	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// DEBUG
	//DebugHelper::renderTex(shadowDepthTex);

	//// Render main
	// Bind world uniforms
	primaryShader.use();
	glUniform4fv(loc_viewPos, 1, glm::value_ptr(glm::vec4(camera.getPosition(), 1.0)));
	glUniformMatrix4fv(loc_viewMat, 1, GL_FALSE, glm::value_ptr(camera.getViewMat()));
	glUniformMatrix4fv(loc_projMat, 1, GL_FALSE, glm::value_ptr(camera.getProjMat()));
	glUniformMatrix4fv(loc_lightMatPrimary, 1, GL_FALSE, glm::value_ptr(light.lightMat));

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	
	// Draw models
	terrain.draw(primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(primaryShader);
	}
}
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
	terrain.generateDiamondSquare(128, 128);

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

	// set up lights
	light = {glm::vec3(500.0, 1000.0, 2000.0), glm::vec4(1.0, 1.0, 1.0, 1.0)};
	light.lightMat = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 1.0f, 10000.0f)
		* glm::lookAt(light.position, glm::vec3(0), glm::vec3(0, 1, 0));

	//// set up shaders
	// primary shader
	string vertexShaderPath = PathHelper::shader("instance.vert");
	string fragmentShaderPath = PathHelper::shader("lighting.frag");
	primaryShader = Shader(vertexShaderPath, fragmentShaderPath);
	loc_viewMat = glGetUniformLocation(primaryShader.getProgramRef(), "viewMat");
	loc_projMat = glGetUniformLocation(primaryShader.getProgramRef(), "projMat");
	GLuint LightPosCLoc = glGetUniformLocation(primaryShader.getProgramRef(), "lightPositionC");
	GLuint LightColorLoc = glGetUniformLocation(primaryShader.getProgramRef(), "lightColor");
	GLuint useLightingLoc = glGetUniformLocation(primaryShader.getProgramRef(), "useLighting");
	primaryShader.use();
	glUniform4fv(LightPosCLoc, 1, glm::value_ptr(camera.getViewMat() 
		* glm::vec4(light.position, 1)));
	glUniform4fv(LightColorLoc, 1, glm::value_ptr(light.color));
	glUniform1i(useLightingLoc, 1);

	// shadowmap shader
	string vertexShadowShaderPath = PathHelper::shader("shadow.vert");
	string fragmentShadowShaderPath = PathHelper::shader("shadow.frag");
	shadowShader = Shader(vertexShadowShaderPath, fragmentShadowShaderPath);
	loc_lightMat = glGetUniformLocation(shadowShader.getProgramRef(), "lightMat");
	shadowShader.use();
	glUniform4fv(loc_lightMat, 1, glm::value_ptr(light.lightMat));

	// shadow depth buffer
	glGenFramebuffers(1, &shadowDepthFBO);
	glGenTextures(1, &shadowDepthTex);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTex, 0); 
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

void World::draw(GLFWwindow* window) {
	//// Render shadow map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// Bind world uniforms
	shadowShader.use();
	glUniformMatrix4fv(loc_lightMat, 1, GL_FALSE, glm::value_ptr(light.lightMat));

	// Draw models
	terrain.draw(primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(primaryShader);
	}

	//// Render main
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);

	// Bind world uniforms
	primaryShader.use();
	glUniformMatrix4fv(loc_viewMat, 1, GL_FALSE, glm::value_ptr(camera.getViewMat()));
	glUniformMatrix4fv(loc_projMat, 1, GL_FALSE, glm::value_ptr(camera.getProjMat()));
	
	// Draw models
	terrain.draw(primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(primaryShader);
	}
}
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

void RenderQuad();

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
	light = {glm::vec3(250.0, 1000.0, 500.0), glm::vec4(1.0, 1.0, 1.0, 1.0)};

	//// set up shaders
	// primary shader
	primaryShader = Shader(PathHelper::shader("instance.vert"), 
		PathHelper::shader("lighting.frag"));
	loc_viewMat = glGetUniformLocation(primaryShader.getProgramRef(), "viewMat");
	loc_projMat = glGetUniformLocation(primaryShader.getProgramRef(), "projMat");
	primaryShader.use();
	glUniform4fv(glGetUniformLocation(primaryShader.getProgramRef(), "lightPositionC"), 
		1, glm::value_ptr(camera.getViewMat() * glm::vec4(light.position, 1)));
	glUniform4fv(glGetUniformLocation(primaryShader.getProgramRef(), "lightColor"), 
		1, glm::value_ptr(light.color));
	glUniform1i(glGetUniformLocation(primaryShader.getProgramRef(), "useLighting"), 1);
	glUniformMatrix4fv(glGetUniformLocation(primaryShader.getProgramRef(), "lightMat"), 
		1, GL_FALSE, glm::value_ptr(light.lightMat));

	// shadowmap shader
	shadowShader = Shader(PathHelper::shader("shadow.vert"), 
		PathHelper::shader("shadow.frag"));
	loc_lightMat = glGetUniformLocation(shadowShader.getProgramRef(), "lightMat");
	shadowShader.use();
	light.lightMat = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, -50.0f, 50.0f)
		* glm::lookAt(glm::normalize(light.position), glm::vec3(0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(loc_lightMat, 1, GL_FALSE, glm::value_ptr(light.lightMat));

	// shadow depth buffer
	glGenFramebuffers(1, &shadowDepthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
	glGenTextures(1, &shadowDepthTex);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTex, 0); 
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer error" << endl;
}

void World::draw(GLFWwindow* window) {
	//// Render shadow map
	// Bind world uniforms
	shadowShader.use();
	glUniformMatrix4fv(loc_lightMat, 1, GL_FALSE, glm::value_ptr(light.lightMat));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

	// Draw models
	terrain.draw(shadowShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(shadowShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// Render main
	// reset viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// bind shadow texture
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
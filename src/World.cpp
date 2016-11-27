#include "World.h"

#include <string>
#include <vector>
#include <random>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Terrain.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "ParaTree.h"
#include "helpers.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Filter.h"
#include "PerlinNoise.h"
#include "Vertex.h"
#include "Skybox.h"
#include "Seeder.h"


// debugging only
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

// constructor

World::World() {
	float terrainHeight = 75, terrainWidth = 250;

	// set up shaders
	primaryShader = new Shader(PathHelper::shader("primary.vert"), 
		PathHelper::shader("primary.frag"));
	shadowShader = new Shader(PathHelper::shader("shadow.vert"), 
		PathHelper::shader("shadow.frag"));

	// set up terrain
	timer.start("Generating terrain...");
	terrain.setShader(primaryShader);
	terrain.generateDiamondSquare(terrainWidth, terrainHeight, 0.4, 0.05, 1, 400);
	timer.stop("Terrain took ");

	// skybox generation
	timer.start("Generating skybox...");
	skybox = new Skybox(1000);
	timer.stop("Skybox took ");

	//// bark generation
	timer.start("Generating tree textures...");
	generateBarkTex();
	timer.stop("Tree textures took ");

	// set up trees
	timer.start("Generating models...");
	Seeder seeder(&terrain, barkTex);
	list<Model*> seeded = seeder.seed(50);
	models.insert(models.end(), seeded.begin(), seeded.end());
	timer.stop("Models took ");

	// set up camera
	glm::vec3 cameraPos(-terrainWidth / 4, 2, -terrainWidth / 4);
	camera.setPosition(glm::vec3(
		cameraPos.x, 
		terrain.getYAtXZWorld(cameraPos.x, cameraPos.z) + cameraPos.y, 
		cameraPos.z));
	camera.lookAt(glm::vec3(0, camera.getPosition().y + 10, 0));

	// set up light
	light = {glm::vec3(250.0, 1000.0, 500.0), glm::vec4(1.0, 1.0, 1.0, 1.0)};

	//// configure shaders
	// primary shader
	loc_viewPos = glGetUniformLocation(primaryShader->getRef(), "viewPos");
	loc_viewMat = glGetUniformLocation(primaryShader->getRef(), "viewMat");
	loc_projMat = glGetUniformLocation(primaryShader->getRef(), "projMat");
	loc_lightMatPrimary = glGetUniformLocation(primaryShader->getRef(), "lightMat");
	loc_shadowTex = glGetUniformLocation(primaryShader->getRef(), "shadowDepthTex");
	primaryShader->use();
	glUniform4fv(glGetUniformLocation(primaryShader->getRef(), "lightPosition"), 
		1, glm::value_ptr(glm::vec4(light.position, 1)));
	glUniform4fv(glGetUniformLocation(primaryShader->getRef(), "lightColor"), 
		1, glm::value_ptr(light.color));
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "useLighting"), 1);
	glUniform1f(glGetUniformLocation(primaryShader->getRef(), "snowLine"), terrainHeight * 3 / 4);
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "PCFSamples"), 
		params.PCFSamples);

	// shadowmap shader
	loc_lightMatShadow = glGetUniformLocation(shadowShader->getRef(), "lightMat");
	
	shadowmapTex = new Texture2D(GL_RG32F, params.shadowWidth, 
		params.shadowHeight, GL_RG, GL_FLOAT, GL_CLAMP_TO_BORDER, Texture::Border(1), 
		Texture::LINEAR);
	shadowmapFBO = new FrameBuffer(shadowmapTex);

	// shadow blur
	blurredShadowmapTex = new Texture2D(*shadowmapTex);
	blurFilter = new BlurFilter(3);
	blurFilter->bind(shadowmapTex, blurredShadowmapTex);

}


// destructor

World::~World() {
	delete skybox;
	delete primaryShader;
	delete shadowShader;
	delete shadowmapFBO;
	delete shadowmapTex;
	delete barkTex;
	delete skyTex;
	delete blurFilter;
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
	light.lightMat = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, -200.0f, 200.0f)
		* glm::lookAt(glm::normalize(light.position), glm::vec3(0), glm::vec3(0, 1, 0));

	//// Render shadow map
	shadowShader->use();
	glUniformMatrix4fv(loc_lightMatShadow, 1, GL_FALSE, glm::value_ptr(light.lightMat));
	glViewport(0, 0, params.shadowWidth, params.shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO->getRef());
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

	// blur shadowmap
	blurFilter->run();

	//// Render main
	// Bind world uniforms
	primaryShader->use();
	glUniform4fv(loc_viewPos, 1, glm::value_ptr(glm::vec4(camera.getPosition(), 1.0)));
	glUniformMatrix4fv(loc_viewMat, 1, GL_FALSE, glm::value_ptr(camera.getViewMat()));
	glUniformMatrix4fv(loc_projMat, 1, GL_FALSE, glm::value_ptr(camera.getProjMat()));
	glUniformMatrix4fv(loc_lightMatPrimary, 1, GL_FALSE, glm::value_ptr(light.lightMat));

	// bind shadow depth texture
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "shadowDepthTex"), 0);
    glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, blurredShadowmapTex->getRef());
	
	// draw skybox
	glm::mat4 skyboxViewMat = camera.getViewMat();
	skyboxViewMat[3][0] = 0;
	skyboxViewMat[3][1] = 0;
	skyboxViewMat[3][2] = 0;
	skybox->draw(skyboxViewMat, camera.getProjMat());

	// Draw models
	terrain.draw(primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(primaryShader);
	}

	// debug quad
	//DebugHelper::renderTex(barkTex);
}


//// protected methods

void World::generateBarkTex() {
	// step-shaded perlin octave noise
	PerlinNoise pnoise(237);
	Texture2D* tempTex1 = new Texture2D(200, 200, [pnoise](float x, float y)->Texture::PixelRGBA32F {
		float val = floor(10.0 * pnoise.octaveNoise(25.0 * x, 5.0 * y, 0, 2, 0.5, 25)) / 10.0;
		return {val, val, val, 1.0};
	});
	//tempTex2->setFilterMode(Texture::LINEAR);

	// find edges
	Texture2D* tempTex2 = new Texture2D(*tempTex1);
	SorbelFilter sorbel;
	sorbel.apply(tempTex1, tempTex2);

	// multiply noise with edges and colorize
	Texture2D* tempTex3 = new Texture2D(*tempTex1);
	barkTex = new Texture2D(*tempTex1);
	Blender blender;
	blender.multiply(tempTex1, tempTex2, tempTex3);
	blender.colorize(tempTex3, glm::vec3(37.0 / 255, 21.0 / 255, 0.0), glm::vec3(131.0 / 255, 83.0 / 255, 25.0 / 255), barkTex);

	delete tempTex1;
	delete tempTex2;
	delete tempTex3;
}
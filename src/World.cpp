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
#include "ParaTree.h"
#include "helpers.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Filter.h"
#include "PerlinNoise.h"
#include "Vertex.h"

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

	// set up textures
	
	DebugHelper::Timer timer;

	//// bark generation
	timer.start();
	generateBarkTex();
	cout << "Timer barkTex: " << timer.stop() << endl;

	// sky texture generation
	timer.start();
	generateSkyTex();
	cout << "Timer skyTex: " << timer.stop() << endl;

	//buildSky();

	// set up trees
	ParaTree* ptree = new ParaTree(ParaTree::Presets::d2, barkTex);
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
	loc_shadowTex = glGetUniformLocation(primaryShader->getRef(), "shadowDepthTex");
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
	light.lightMat = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -200.0f, 200.0f)
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
    //blurredShadowmapTex->bind(1);
    //glUniform1i(loc_shadowTex, 1);
	glUniform1i(glGetUniformLocation(primaryShader->getRef(), "shadowDepthTex"), 0);
    glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, blurredShadowmapTex->getRef());
	
	// Draw models
	terrain.draw(primaryShader);
	for(list<Model*>::iterator it = models.begin(); it != models.end(); it++) {
		(*it)->draw(primaryShader);
	}

	// debug quad
	DebugHelper::renderTex(skyTex);
}


//// protected methods

void World::generateBarkTex() {
	// step-shaded perlin octave noise
	PerlinNoise pnoise(237);
	Texture2D* tempTex1 = new Texture2D(200, 200, [pnoise](float x, float y)->Texture::PixelRGBA32F {
		float val = floor(10.0 * pnoise.octaveNoise(25.0 * x, 5.0 * y, 0, 2, 0.5)) / 10.0;
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

void World::generateSkyTex() {
	PerlinNoise pnoise(237);
	//TextureCubemap* skybox = new TextureCubemap();


	Texture2D* tempTex1 = new Texture2D(600, 600, [pnoise](float x, float y)->Texture::PixelRGBA32F {
		float xPersp = x / y;
		float yPersp = 1.0 / y;
		float val = min(1.0, 
			  max(0.0, pnoise.octaveNoise(15.0 * xPersp, 15.0 * yPersp, 0, 8, 0.5) - 0.5) * 2.0
			* max(0.0, pnoise.octaveNoise(5.0 * xPersp, 5.0 * yPersp, 0, 2, 0.5) - 0.4)
			* 10.0 / 6.0 * 3);
		return {val, val, val, 1.0};
	});
	//skyTex = new Texture2D(*tempTex1);
	skyTex = tempTex1;
}

void World::buildSky() {
	Model* sky = new Model();
	vector<Vertex> vertices;
	vertices.push_back(Vertex{
		glm::vec3(-10000, 0, -10000),
		glm::vec3(0, -1, 0),
		glm::vec2(0, 0)
	});
	vertices.push_back(Vertex{
		glm::vec3(10000, 0, -10000),
		glm::vec3(0, -1, 0),
		glm::vec2(1, 0)
	});
	vertices.push_back(Vertex{
		glm::vec3(-10000, 0, 10000),
		glm::vec3(0, -1, 0),
		glm::vec2(0, 1)
	});
	vertices.push_back(Vertex{
		glm::vec3(10000, 0, 10000),
		glm::vec3(0, -1, 0),
		glm::vec2(1, 1)
	});
	Mesh* mesh = sky->newMesh(vertices, GL_TRIANGLE_STRIP);
	mesh->newInstance();
	models.push_back(sky);
	sky->translate(glm::vec3(0, 1500, 0));
}
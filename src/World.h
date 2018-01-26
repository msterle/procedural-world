#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/glm.hpp"

#include "Shader.h"
#include "Terrain.h"
#include "Camera.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Filter.h"
#include "Skybox.h"
#include "ParaTree.h"
#include "helpers.h"

using namespace std;

struct Light {
	glm::vec3 position;
	glm::vec4 color;
	glm::mat4 lightMat;
};

class World {
public:
	World();
	~World();
	void draw(GLFWwindow* window);
	unsigned int generationInc() {
		this->generation++;
		this->generateDynamicTrees();
		return this->generation;
	};
	unsigned int generationDec() {
		if(this->generation > 0) --this->generation;
		this->generateDynamicTrees();
		return this->generation;
	};
	Terrain terrain;
	Skybox* skybox;
	Camera camera;
	struct {
		GLuint shadowWidth = 2048;
		GLuint shadowHeight = 2048;
		GLuint PCFSamples = 0;
	} params;
	list<Model*> getModels() { 
		list<Model*> newModel = models;
		return newModel;
	}
protected:
	unsigned int generation = 6;
	glm::vec3 originGround;
	vector<ParaTree*> dynamicTrees;
	const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
	const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";
	GLint loc_viewPos, loc_viewMat, loc_projMat, loc_lightMatShadow, loc_lightMatPrimary, loc_shadowTex;
	list<Model*> models;
	Light light;
	Shader* primaryShader, * shadowShader;
	FrameBuffer* shadowmapFBO;
	Texture2D* shadowmapTex, * blurredShadowmapTex, * barkTex, * leafTex, * skyTex;
	BlurFilter* blurFilter;
	DebugHelper::Timer timer;
	void generateBarkTex();
	void generateTrees(unsigned int count);
	void generateDynamicTrees();
};

#endif
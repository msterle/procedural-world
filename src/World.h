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
	Terrain terrain;
	Camera camera;
	struct {
		GLuint shadowWidth = 1024;
		GLuint shadowHeight = 1024;
		GLuint PCFSamples = 0;
	} params;
protected:
	const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
	const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";
	GLint loc_viewPos, loc_viewMat, loc_projMat, loc_lightMatShadow, loc_lightMatPrimary, loc_shadowTex;
	list<Model*> models;
	Light light;
	Shader* primaryShader, * shadowShader;
	FrameBuffer* shadowmapFBO;
	Texture2D* shadowmapTex, * blurredShadowmapTex, * barkTex, * skyTex;
	BlurFilter* blurFilter;
	void generateBarkTex();
	void generateSkyTex();
	void buildSky();
};

#endif
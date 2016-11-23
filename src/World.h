#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/glm.hpp"

#include "Shader.h"
#include "Terrain.h"
#include "Camera.h"

using namespace std;

struct Light {
	glm::vec3 position;
	glm::vec4 color;
	glm::mat4 lightMat;
};

class World {
public:
	World();
	void draw(GLFWwindow* window);
	Terrain terrain;
	Camera camera;
	list<Model*> models;
	struct {
		GLuint shadowWidth = 2048;
		GLuint shadowHeight = 2048;
		GLuint shadowSamples = 0;
	} params;
protected:
	const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
	const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";
	Shader primaryShader, shadowShader;
	GLint loc_viewPos, loc_viewMat, loc_projMat, loc_lightMatShadow, loc_lightMatPrimary;
	Light light;
	GLuint shadowDepthFBO, shadowDepthTex;
};

#endif
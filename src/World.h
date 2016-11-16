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

const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

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
protected:
	const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
	const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";
	Shader primaryShader, shadowShader, depthShader;
	GLint loc_viewPos, loc_viewMat, loc_projMat, loc_lightMat;
	list<Model*> models;
	Light light;
	GLuint shadowDepthFBO, shadowDepthTex;
};

#endif
#ifndef WORLD_H
#define WORLD_H

#include "../include/glew.h"
#include "../include/glfw3.h"

#include "Shader.h"
#include "Terrain.h"
#include "Camera.h"

class World {
public:
	World();
	void draw();
	Terrain terrain;
	Camera camera;
protected:
	const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
	const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";
	GLFWwindow* window;
	Shader shader;
	GLint uni_viewMat, uni_projMat;
};

#endif
#ifndef WORLD_H
#define WORLD_H

#include "../include/glew.h"
#include "../include/glfw3.h"

#include "shader.h"
#include "terrain.h"
#include "camera.h"

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
	ShaderProgram shaderProgram;
	GLint uni_viewMat, uni_projMat;
};

#endif
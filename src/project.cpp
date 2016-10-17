#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <string>

#include "glhelper.h"
#include "shader.h"
#include "terrain.h"
#include "camera.h"
#include "world.h"
#include "ui.h"

using namespace std;

int main() {
	//// Program setup
	GLFWwindow* window = glhelper::initGL();
	
	World* world = new World();
	UI::init(world, &world->camera, window);
	UI::setActive(UI::walk);

	// Main loop
	while ( !glfwWindowShouldClose(window) ) {
		glfwPollEvents();
		world->draw();
	}

	// Cleanup before exit
	delete world;
	glfwTerminate();
	return 0;
}
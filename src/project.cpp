#include "../include/glew.h"
#include "../include/glfw3.h"
#include <iostream>
#include <string>

#include "glhelper.h"
#include "world.h"
#include "ui.h"

using namespace std;

int main() {
	//// Program setup
	GLFWwindow* window = glhelper::initGL();
	World* world = new World();
	UI::init(world, window);
	UI::setActive(UI::Explore);

	// Main loop
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		world->draw();
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Cleanup before exit
	delete world;
	glfwTerminate();
	return 0;
}
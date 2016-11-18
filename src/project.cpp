#include "../include/glew.h"
#include "../include/glfw3.h"
#include <iostream>
#include <string>

#include "helpers.h"
#include "World.h"
#include "UI.h"

using namespace std;

int main() {
	//// Program setup
	GLFWwindow* window = GLHelper::initGL();
	World* world = new World();
	UI::init(world, window);
	UI::setActive(UI::Explore);

	double curTime = glfwGetTime(), lastTime, deltaTime, cumTime = 0;
	int frameCount = 0;
	float fps = 0;

	// Main loop
	while(!glfwWindowShouldClose(window)) {
		lastTime = curTime;
		curTime = glfwGetTime();
		deltaTime = curTime - lastTime;
		cumTime += deltaTime;
		if(++frameCount >= 30) {
			fps = (float)frameCount / cumTime;
			cumTime = 0;
			frameCount = 0;
			cout << "FPS: " << fps << endl;
		}
		glfwPollEvents();
		world->draw(window);
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Cleanup before exit
	glfwTerminate();
	return 0;
}
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

using namespace std;

const float CAMERA_FOV = 45.0f; // camera field of view

const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";

// Mouse sensitivity
const float MOUSE_SENS_MOV = 0.005f;

// Type definition for state data to be passed to GLFW calls
struct stateData_t {
	// Model, view and projection transformation matrices
	glm::mat4 model, view, proj;
	Terrain terrain;
	cimg_library::CImg<unsigned char> heightImg, colorImg;
	Camera camera;
	// Last recorded cursor positions
	double cursorLastX, cursorLastY;
};

// Window resize callback
void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
	stateData->camera.perspective(glm::radians(CAMERA_FOV),
		(float)width / (float)height,
		0.1f,
		10.0f
		);
}

// Keyboard event callback
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if(action == GLFW_PRESS || action == GLFW_REPEAT) {
		stateData_t* stateData;
		switch(key) {
		// Rotate model around world axes
		case GLFW_KEY_X:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.rotate(glm::radians(10.0f),
				glm::vec3((mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f, 0.0f ));
			break;
		case GLFW_KEY_Y:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.rotate(glm::radians(10.0f), 
				glm::vec3(0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f));
			break;
		case GLFW_KEY_Z:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.rotate(glm::radians(10.0f), 
				glm::vec3(0.0f, 0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f)));
			break;
		case GLFW_KEY_LEFT:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.moveRelative(glm::vec3(-0.1, 0, 0));
			break;
		case GLFW_KEY_RIGHT:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.moveRelative(glm::vec3(0.1, 0, 0));
			break;
		case GLFW_KEY_UP:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.moveRelative(glm::vec3(0, 0.1, 0));
			break;
		case GLFW_KEY_DOWN:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.moveRelative(glm::vec3(0, -0.1, 0));
			break;
		case GLFW_KEY_T:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case GLFW_KEY_W:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		case GLFW_KEY_P:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			break;
		case GLFW_KEY_1:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.resampleHeightmap(1);
			break;
		case GLFW_KEY_2:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.resampleHeightmap(2);
			break;
		case GLFW_KEY_4:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.resampleHeightmap(4);
			break;
		}
	}
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if( action == GLFW_PRESS && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) ) {
		// store cursor position
		stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
		glfwGetCursorPos(window, &stateData->cursorLastX, &stateData->cursorLastY);
	}
}

static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		// change heading and attitude
		stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float anglePerPixel = CAMERA_FOV / height;
		stateData->camera.yaw((stateData->cursorLastX - xpos) * anglePerPixel);
		stateData->camera.pitch((ypos - stateData->cursorLastY) * anglePerPixel);
		stateData->cursorLastX = xpos;
		stateData->cursorLastY = ypos;
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
		// move camera in/out
		stateData->camera.moveRelative(glm::vec3(0, 0, stateData->cursorLastY - ypos) * MOUSE_SENS_MOV);
		// store cursor position
		stateData->cursorLastX = xpos;
		stateData->cursorLastY = ypos;
	}
}



int main() {
	//// Program setup
	GLFWwindow* window = glhelper::initGL();

	// Set to catch state change between polling cycles
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	// Set window callback functions
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);

	stateData_t* stateData = new stateData_t;
	glfwSetWindowUserPointer(window, stateData);

	string vertexShaderPath = string(PROJECT_ROOT) + string("/src/shaders/vertex.shader");
	string fragmentShaderPath = string(PROJECT_ROOT) + string("/src/shaders/fragment.shader");
	ShaderProgram shaderProgram(vertexShaderPath, fragmentShaderPath);

	//// Data setup
	string terrainHeightmapPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_HEIGHTMAP);
	string terrainColorPath = string(PROJECT_ROOT) + string(TERRAIN_PATH_COLOR);

	cout << "Creating terrain..." << endl;
	stateData->terrain.setShaderProgram(shaderProgram.getProgramRef());
	stateData->terrain.buildFromHeightmap(terrainHeightmapPath, terrainColorPath);

	// position camera
	stateData->camera.setPosition(glm::vec3(1, 1, 1));
	stateData->camera.lookAt(glm::vec3(0, 0, 0));

	// Initialize projection matrix
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	stateData->camera.perspective(
		glm::radians(CAMERA_FOV),
		(float)width / (float)height,
		0.1f, 
		10.0f
		);

	// Get shader matrix pointers
	GLint uniView = glGetUniformLocation(shaderProgram.getProgramRef(), "view");
	GLint uniProj = glGetUniformLocation(shaderProgram.getProgramRef(), "proj");

	// Main loop
	while ( !glfwWindowShouldClose(window) ) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update view matrix from camera
		stateData->view = stateData->camera.getViewMat();

		// Bind transformation matrices to shader program
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(stateData->camera.getViewMat()));
		glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(stateData->camera.getProjMat()));

		stateData->terrain.draw();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Cleanup before exit
	delete stateData;
	glfwTerminate();
	return 0;
}

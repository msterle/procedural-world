#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "terrain.cpp"
#include "camera.cpp"
#include "shaders.cpp"

using namespace std;

// Target openGL version
const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;

// Window dimensions
const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

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

// Error handling callback
void glfw_error_callback(int error, const char* description) {
	cerr << "GLFW error: " << description << endl;
}

// Window resize callback
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
	// Modify projection matrix
	stateData->proj = glm::perspective(
		glm::radians(CAMERA_FOV),
		(float)width / (float)height,
		0.1f,
		10.0f
		);
	// Update viewport
	glViewport(0, 0, width, height);
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
			stateData->terrain.buildFromHeightmap(stateData->heightImg, stateData->colorImg, 1);
			break;
		case GLFW_KEY_2:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.buildFromHeightmap(stateData->heightImg, stateData->colorImg, 2);
			break;
		case GLFW_KEY_4:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->terrain.buildFromHeightmap(stateData->heightImg, stateData->colorImg, 4);
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

GLFWwindow* initGL() {
	//// GLFW setup

	cout << "Starting GLFW context, OpenGL " << OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR << endl;

	// Initialize GLFW
	glfwInit();
	glfwSetErrorCallback(glfw_error_callback);

	// Set GLFW options
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Triangle", nullptr, nullptr);
	if (window == nullptr) {
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	// Set to catch state change between polling cycles
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

	// Set window callback functions
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);


	//// GLEW setup

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW" << endl;
		exit(-1);
	}

	// Initialize the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//// OpenGL setup

	// Set point size for point draw mode
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(1.0f);

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable primitive restart
	glEnable(GL_PRIMITIVE_RESTART);
	// Define restart index
	glPrimitiveRestartIndex(65535);

	return window;
}

int main() {

	//// Program setup
	GLFWwindow* window = initGL();

	stateData_t* stateData = new stateData_t;
	glfwSetWindowUserPointer(window, stateData);

	string vertexShaderPath = string(PROJECT_ROOT) + string("/src/shaders/vertex.shader");
	string fragmentShaderPath = string(PROJECT_ROOT) + string("/src/shaders/fragment.shader");

	ShaderProgram shaderProgram(vertexShaderPath, fragmentShaderPath);

	//// Data setup
	stateData->heightImg = cimg_library::CImg<unsigned char>((string(PROJECT_ROOT) + string(TERRAIN_PATH_HEIGHTMAP)).c_str());
	stateData->colorImg = cimg_library::CImg<unsigned char>((string(PROJECT_ROOT) + string(TERRAIN_PATH_COLOR)).c_str());

	cout << "Creating terrain..." << endl;
	stateData->terrain.setShaderProgram(shaderProgram.getProgramRef());
	stateData->terrain.buildFromHeightmap(stateData->heightImg, stateData->colorImg);

	// position camera
	stateData->camera.setPosition(glm::vec3(1, 1, 1));
	stateData->camera.lookAt(glm::vec3(0, 0, 0));

	// Initialize projection matrix
	stateData->proj = glm::perspective(
		glm::radians(CAMERA_FOV),
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		0.005f, 10.0f
		);

	// Get shader matrix pointers
	GLint uniTrans = glGetUniformLocation(shaderProgram.getProgramRef(), "model");
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
		glUniformMatrix4fv( uniTrans, 1, GL_FALSE, glm::value_ptr(stateData->model) );
		glUniformMatrix4fv( uniView, 1, GL_FALSE, glm::value_ptr(stateData->view) );
		glUniformMatrix4fv( uniProj, 1, GL_FALSE, glm::value_ptr(stateData->proj) );

		stateData->terrain.draw();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Cleanup before exit
	delete stateData;
	glfwTerminate();
	return 0;
}

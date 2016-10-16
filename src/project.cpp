#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/CImg.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/ext.hpp"
#include "../include/glm/gtc/constants.hpp"

#include "terrain.cpp"

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

// Target openGL version
const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;

// Window dimensions
const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;

const char* TERRAIN_PATH_HEIGHTMAP = "/res/heightmap_lores.png";
const char* TERRAIN_PATH_COLOR = "/res/colour_lores.png";

// Vertical scaling for terrain
const float VSCALE = 0.25;

// Mouse sensitivity
const float MOUSE_SENS_MOV = 0.005f;
const float MOUSE_SENS_ROT = 0.001f;

// Type definition for state data to be passed to GLFW calls
struct stateData_t {
	GLuint shaderProgram;
	// Model, view and projection transformation matrices
	glm::mat4 model, view, proj;
	Terrain terrain;
	cimg_library::CImg<unsigned char> heightImg, colorImg;
	// Camera definition
	struct {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 up;
	} camera;
	// Last recorded cursor positions
	double cursorLastX, cursorLastY;
};

// Build and compile shader program
GLuint buildShaderProgram() {
	// Read the Vertex Shader code from the file
	string vertex_shader_path = string(PROJECT_ROOT) + string("/src/shaders/vertex.shader");
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path.c_str(), ios::in);

	if ( VertexShaderStream.is_open() ) {
		string Line = "";
		while ( getline(VertexShaderStream, Line) )
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		cerr << "Error: Couldn't open vertex shader program ''" << vertex_shader_path.c_str() << "'" << endl;
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = string(PROJECT_ROOT) + string("/src/shaders/fragment.shader");
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path.c_str(), std::ios::in);

	if ( FragmentShaderStream.is_open() ) {
		std::string Line = "";
		while ( getline(FragmentShaderStream, Line) )
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		cerr << "Error: Couldn't open fragment shader program ''" << fragment_shader_path.c_str() << "'" <<
		        endl;
		exit(-1);
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);

	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << infoLog << endl;
		exit(-1);
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		exit(-1);
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);
	return shaderProgram;
}

// Error handling callback
void glfw_error_callback(int error, const char* description) {
	cerr << "GLFW error: " << description << endl;
}

// Window resize callback
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
	// Modify projection matrix
	stateData->proj = glm::perspective(
		glm::radians(45.0f),
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
			stateData->camera.position -= 0.1f * glm::cross(stateData->camera.direction,
			                                                stateData->camera.up);
			break;
		case GLFW_KEY_RIGHT:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.position += 0.1f * glm::cross(stateData->camera.direction,
			                                                stateData->camera.up);
			break;
		case GLFW_KEY_UP:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.position += 0.1f * stateData->camera.up;
			break;
		case GLFW_KEY_DOWN:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->camera.position -= 0.1f * stateData->camera.up;
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
		stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
		// convert to euler angles
		float pitch =
			glm::atan( stateData->camera.direction.z,
			           (float)glm::sqrt( glm::pow(stateData->camera.direction.x,
			                                      2) + glm::pow(stateData->camera.direction.y, 2) ) ),
		      yaw = glm::atan(stateData->camera.direction.y, stateData->camera.direction.x);
		// update euler angles
		yaw += (xpos - stateData->cursorLastX) * MOUSE_SENS_ROT;
		pitch += (ypos - stateData->cursorLastY) * MOUSE_SENS_ROT;
		if(pitch > glm::half_pi<float>() - 0.01) pitch = glm::half_pi<float>() - 0.01;
		if(pitch < -glm::half_pi<float>() + 0.01) pitch = -glm::half_pi<float>() + 0.01;
		// convert back to direction vector
		stateData->camera.direction.x = cos(pitch) * cos(yaw);
		stateData->camera.direction.y = cos(pitch) * sin(yaw);
		stateData->camera.direction.z = sin(pitch);
		// recalculate up vector
		stateData->camera.up = glm::normalize(
			glm::cross(
				glm::cross( stateData->camera.direction, glm::vec3(0.0f, 0.0f, 0.1f) ) // find camera right first
				, stateData->camera.direction) );
		// store cursor position
		stateData->cursorLastX = xpos;
		stateData->cursorLastY = ypos;
	}
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		stateData_t* stateData = (stateData_t*)glfwGetWindowUserPointer(window);
		// move camera in/out
		stateData->camera.position += stateData->camera.direction * (stateData->cursorLastY - ypos) *
		                              MOUSE_SENS_MOV;
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
	stateData->shaderProgram = buildShaderProgram();

	//// Data setup
	stateData->heightImg = cimg_library::CImg<unsigned char>((string(PROJECT_ROOT) + string(TERRAIN_PATH_HEIGHTMAP)).c_str());
	stateData->colorImg = cimg_library::CImg<unsigned char>((string(PROJECT_ROOT) + string(TERRAIN_PATH_COLOR)).c_str());

	cout << "Creating terrain..." << endl;
	stateData->terrain.setShaderProgram(stateData->shaderProgram);
	stateData->terrain.buildFromHeightmap(stateData->heightImg, stateData->colorImg);

	// Initialize camera and view matrix
	stateData->camera.position = glm::vec3(1.0f, 1.0f, 1.0f);
	stateData->camera.direction = glm::normalize(-stateData->camera.position); // Set to look at (0, 0, 0)
	stateData->camera.up = glm::normalize(
		glm::cross(
			glm::cross( stateData->camera.direction, glm::vec3(0.0f, 0.0f, 0.1f) ) // find camera right first
			, stateData->camera.direction) );

	stateData->view = glm::lookAt(
		stateData->camera.position,
		stateData->camera.position + stateData->camera.direction,
		stateData->camera.up
		);

	// Initialize projection matrix
	stateData->proj = glm::perspective(
		glm::radians(45.0f),
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
		0.005f, 10.0f
		);

	// Get shader matrix pointers
	GLint uniTrans = glGetUniformLocation(stateData->shaderProgram, "model");
	GLint uniView = glGetUniformLocation(stateData->shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(stateData->shaderProgram, "proj");

	// Main loop
	while ( !glfwWindowShouldClose(window) ) {
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update view matrix from camera
		stateData->view = glm::lookAt(
			stateData->camera.position,
			stateData->camera.position + stateData->camera.direction,
			stateData->camera.up
			);

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

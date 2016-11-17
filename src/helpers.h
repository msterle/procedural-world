#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"

namespace GLHelper {
	const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;
	const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	
	inline void glfw_error_callback(int error, const char* description) {
		std::cerr << "GLFW error: " << description << std::endl;
	}

	inline GLFWwindow* initGL() {
		//// GLFW setup

		std::cout << "Starting GLFW context, OpenGL " << OPENGL_VERSION_MAJOR << "." << OPENGL_VERSION_MINOR << std::endl;

		// Initialize GLFW
		glfwInit();
		glfwSetErrorCallback(glfw_error_callback);

		// Set GLFW options
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 4); // antialiasing

		// Create a GLFWwindow object that we can use for GLFW's functions
		GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "COMP 371 Project", nullptr, nullptr);
		if (window == nullptr) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);

		//// GLEW setup

		// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
		glewExperimental = GL_TRUE;

		// Initialize GLEW to setup the OpenGL Function pointers
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Failed to initialize GLEW" << std::endl;
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

		// antialiasing
		glEnable(GL_MULTISAMPLE);

		return window;
	}
}

namespace PathHelper {
	inline std::string root() { return std::string(PROJECT_ROOT); }
	inline std::string absolute(std::string path = "") { return root() + '/' + path; }
	inline std::string shader(std::string path = "") { return root() + "/src/shaders/" + path; }
	inline std::string res(std::string path = "") { return root() + "/res/" + path; }
	inline std::string model(std::string path = "") { return res() + "/models/" + path; }
}

#endif
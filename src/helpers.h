#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Shader.h"

namespace GLHelper {
	const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;
	const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	GLFWwindow* initGL();
	inline void glfw_error_callback(int error, const char* description) {
		std::cerr << "GLFW error: " << description << std::endl;
	}
}

namespace PathHelper {
	inline std::string root() { return std::string(PROJECT_ROOT); }
	inline std::string absolute(std::string path = "") { return root() + '/' + path; }
	inline std::string shader(std::string path = "") { return root() + "/src/shaders/" + path; }
	inline std::string res(std::string path = "") { return root() + "/res/" + path; }
	inline std::string model(std::string path = "") { return res() + "/models/" + path; }
}

namespace DebugHelper {
	void renderTex(GLuint tex);
}

#endif
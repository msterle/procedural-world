#ifndef GLHELPER_H
#define GLHELPER_H

#include "../include/glew.h"
#include "../include/glfw3.h"

namespace glhelper {
	const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;
	const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	GLFWwindow* initGL();
	void glfw_error_callback(int error, const char* description);
}

#endif
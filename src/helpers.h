#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Shader.h"
#include "Texture.h"

namespace GLHelper {
	struct Error {
		GLenum code;
		std::string name() { return to_string(code); }
	private:
		static std::string to_string(GLenum err) {
			switch(err) {
				case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
				case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
				case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
				case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
				case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
				case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
				case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
				case GL_CONTEXT_LOST:                  return "GL_CONTEXT_LOST";
				case GL_TABLE_TOO_LARGE:               return "GL_TABLE_TOO_LARGE";
				default:                               return "Unknown error code";
			}
		}
	};
	const int OPENGL_VERSION_MAJOR = 3, OPENGL_VERSION_MINOR = 3;
	const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
	GLFWwindow* initGL();
	Error getError();
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
	void renderTex(Texture2D* tex, float scale = 1);
	struct Timer {
		double t0;
		void start() { t0 = glfwGetTime(); }
		double stop() { return glfwGetTime() - t0; }
	};
}

#endif
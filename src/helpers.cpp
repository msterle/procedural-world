#include "helpers.h"
#include <string>
#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Shader.h"

namespace GLHelper{

	GLFWwindow* initGL() {
		//// GLFW setup

		std::cout << "Starting GLFW context, OpenGL " << OPENGL_VERSION_MAJOR 
			<< "." << OPENGL_VERSION_MINOR << std::endl;

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
		GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 
			"COMP 371 Project", nullptr, nullptr);
		if (window == nullptr) {
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);

		//// GLEW setup

		// Set this to true so GLEW knows to use a modern approach to 
		// retrieving function pointers and extensions
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

namespace DebugHelper {

	void renderTex(GLuint tex) {
		static GLuint texVAO = 0, texVBO;
		static Shader texShader;
		if (texVAO == 0){
			texShader = Shader(PathHelper::shader("debug_tex.vert"),
				PathHelper::shader("debug_tex.frag"));
			GLfloat quadVertices[] = {
				// Positions        // Texture Coords
				-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
				1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			};
			// Setup plane VAO
			glGenVertexArrays(1, &texVAO);
			glGenBuffers(1, &texVBO);
			glBindVertexArray(texVAO);
			glBindBuffer(GL_ARRAY_BUFFER, texVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
			(GLvoid*)(3 * sizeof(GLfloat)));
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		texShader.use();
		glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, tex);
		glBindVertexArray(texVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}
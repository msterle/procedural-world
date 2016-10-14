#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/CImg.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/ext.hpp"
#include "../include/glm/gtc/constants.hpp"

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

// Vertical scaling for terrain
const float VSCALE = 0.25;

// Mouse sensitivity
const float MOUSE_SENS_MOV = 0.005f;
const float MOUSE_SENS_ROT = 0.001f;

// Type definition for state data to be passed to GLFW calls
struct stateData_t {
	GLuint VAO, VBO, EBO;
	GLuint shaderProgram;
	// Model, view and projection transformation matrices
	glm::mat4 model, view, proj;
	// Vertex buffer type definition/instantiation
	struct vertexArray_t {
		int width;
		int height;
		vector<GLfloat> vertices;
	} vertexArray;
	vector<GLuint> indexArray;
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

float upsample(int x, int y, int c, cimg_library::CImg<unsigned char>* image, int width, int height, int scale) {
	// Weighted average of four surrounding source pixels
	// note: requires image width & height to reduce overhead of calling CImg methods
	// ex: in 4x4 upsampling (scale = 4), output pixel (1, 2) has weighted average of
	// surrounding 4 input pixels A (0,0), B (1, 0), C(0, 1) and D (1, 1)
	// 6/16 * A + 2/16 * B + 6/16 * C + 2/16 * D
	int xmod = x % scale, ymod = y % scale;
	int xLast = x / scale + 1 < width ? 1 : 0;
	int yLast = y / scale + 1 < height ? 1 : 0;
	// Optimized from operator() to
	return ( (float)*image->data(x / scale,         y / scale,         0, c) * (scale - xmod) * (scale - ymod) +
	         (float)*image->data(x / scale + xLast, y / scale,         0, c) * xmod *           (scale - ymod) +
	         (float)*image->data(x / scale,         y / scale + yLast, 0, c) * (scale - xmod) * ymod +
	         (float)*image->data(x / scale + xLast, y / scale + yLast, 0, c) * xmod *           ymod ) /
	       (scale * scale);     // faster than pow(scale, 2)
}

// Build vertex array from input files: res/heightmap.png for height values and res/colour.png for colour values
void buildVertexArray(stateData_t* stateData, int scale = 1) {
	// vertex format [x, y, z, r, g, b]
	// Load source images
	cimg_library::CImg<unsigned char> heightImg((string(PROJECT_ROOT) + string("/res/heightmap.png")).c_str()), colourImg((string(PROJECT_ROOT) + string("/res/colour.png")).c_str());
	if( heightImg.width() != colourImg.width() || heightImg.height() != colourImg.height() ) {
		cerr << "Error: input images not equal dimensions";
		exit(-1);
	}
	int sourceWidth = heightImg.width(), sourceHeight = heightImg.width();
	stateData->vertexArray.width = scale * sourceWidth;
	stateData->vertexArray.height = scale * sourceHeight;
	// Clear and allocate vector
	stateData->vertexArray.vertices.clear();
	stateData->vertexArray.vertices.resize(stateData->vertexArray.width * stateData->vertexArray.height * 6);
	float maxZ = 0;
	for(int y = 0; y < stateData->vertexArray.height; y++) {
		for(int x = 0; x < stateData->vertexArray.width; x++) {
			// For each pixel add a vertex with pixel's x and y values, z determined
			// from heightmap image's first channel value and colour copied from colour image
			// Optimized from push_back to operator[]
			stateData->vertexArray.vertices[6 * (y * stateData->vertexArray.width + x)] =
				(float) x / (stateData->vertexArray.width - 1) - 0.5f;
			stateData->vertexArray.vertices[6 * (y * stateData->vertexArray.width + x) + 1] =
				(float) y / (stateData->vertexArray.height - 1) -0.5f;
			float z = upsample(x, y, 0, &heightImg, sourceWidth, sourceHeight, scale) / 255 * VSCALE;
			// Determine maximum z value
			if(z > maxZ)
				maxZ = z;
			stateData->vertexArray.vertices[6 * (y * stateData->vertexArray.width + x) + 2] = z;
			int channels[3];
			// For colour upsampling, average of four surrounding source pixels weighted accordingly
			for(int c = 0; c < 3; c++)
				stateData->vertexArray.vertices[6 * (y * stateData->vertexArray.width + x) + 3 +
				                                c] = upsample(x, y, c, &colourImg, sourceWidth,
				                                              sourceHeight, scale) / 255;
		}
	}

	// Center object on Z plane according to maximum z value
	for(int y = 0; y < stateData->vertexArray.height; y++) {
		for(int x = 0; x < stateData->vertexArray.width; x++) {
			stateData->vertexArray.vertices[(y * stateData->vertexArray.width + x) * 6 + 2] -= maxZ / 2;
		}
	}
}

// Build element index array
void buildIndexBuffer(stateData_t* stateData) {
	// Clear and allocate vector
	stateData->indexArray.clear();
	stateData->indexArray.reserve( (stateData->vertexArray.width * 2 + 1) * stateData->vertexArray.height );
	// Not worth optimizing push_back call
	for(int y = 0; y < stateData->vertexArray.height - 1; y++) {
		for(int x = 0; x < stateData->vertexArray.width; x++) {
			stateData->indexArray.push_back(x + y * stateData->vertexArray.width);
			stateData->indexArray.push_back(x + (y + 1) * stateData->vertexArray.width);
		}
		// Restart primitive at end of row
		if(y < stateData->vertexArray.height - 2) {
			stateData->indexArray.push_back(65535);
		}
	}
}

void bindBuffers(stateData_t* stateData) {
	glBindBuffer(GL_ARRAY_BUFFER, stateData->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stateData->EBO);

	// Set vertex buffer data
	glBufferData(GL_ARRAY_BUFFER,
	             stateData->vertexArray.vertices.size() * sizeof(GLfloat),
	             &stateData->vertexArray.vertices.front(), GL_STATIC_DRAW);

	// Set attribute pointers
	// model: [x, y, z, r, g, b]
	GLint posAttrib = glGetAttribLocation(stateData->shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(stateData->shaderProgram, "in_colour");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)( 3 * sizeof(GLfloat) ) );

	// Set index buffer data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	             stateData->indexArray.size() * sizeof(GLuint), &stateData->indexArray.front(), GL_STATIC_DRAW);

	// Unbind array buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void buildBuffers(stateData_t* stateData, int scale = 1) {
	buildVertexArray(stateData, scale);
	buildIndexBuffer(stateData);
	bindBuffers(stateData);
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
			stateData->model = glm::rotate(
				glm::mat4(),
				glm::radians(10.0f),
				glm::vec3( (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f, 0.0f )
				) * stateData->model;
			break;
		case GLFW_KEY_Y:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->model = glm::rotate(
				glm::mat4(),
				glm::radians(10.0f),
				glm::vec3(0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f)
				) * stateData->model;
			break;
		case GLFW_KEY_Z:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			stateData->model = glm::rotate(
				glm::mat4(),
				glm::radians(10.0f),
				glm::vec3( 0.0f, 0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f) )
				) * stateData->model;
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
			buildBuffers(stateData, 1);
			break;
		case GLFW_KEY_2:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			buildBuffers(stateData, 2);
			break;
		case GLFW_KEY_4:
			stateData = (stateData_t*)glfwGetWindowUserPointer(window);
			buildBuffers(stateData, 4);
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

	// Initialize vertex array
	glGenVertexArrays(1, &stateData->VAO);
	glBindVertexArray(stateData->VAO);

	// Initialize vertex buffer
	glGenBuffers(1, &stateData->VBO);

	// Initialize element buffer
	glGenBuffers(1, &stateData->EBO);

	// Build buffers
	buildBuffers(stateData, 1);

	glBindVertexArray(0);

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

		// Draw elements
		glBindVertexArray(stateData->VAO);
		glDrawElements(GL_TRIANGLE_STRIP, stateData->indexArray.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Cleanup before exit
	delete stateData;
	glfwTerminate();
	return 0;
}

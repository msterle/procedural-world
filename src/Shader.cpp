#include "../include/glew.h"
#include "../include/glfw3.h"
#include <string>
#include <fstream>

#include "Shader.h"

// Debugging only
#include <iostream>

using namespace std;


// Constructors

Shader::Shader() {
	
}

Shader::Shader(string vertexShaderPath, string fragmentShaderPath) {
	// Compile shaders
	GLuint vertexShaderRef = this->compileShader(VertexShader, vertexShaderPath);
	GLuint fragmentShaderRef = this->compileShader(FragmentShader, fragmentShaderPath);
	
	// Link shaders
	this->programID = glCreateProgram();
	glAttachShader(this->programID, vertexShaderRef);
	glAttachShader(this->programID, fragmentShaderRef);
	glLinkProgram(this->programID);

	// Check for linking errors
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(this->programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->programID, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		exit(-1);
	}
	glDeleteShader(vertexShaderRef);
	glDeleteShader(fragmentShaderRef);
}


// Protected methods

GLuint Shader::compileShader(Type shaderType, string path) {
	string shaderTypeString = shaderType == VertexShader ? "VertexShader" : "FragmentShader";
	// Read the shader code from the file
	string shaderCode;
	ifstream programStream(path.c_str(), ios::in);

	if (programStream.is_open() ) {
		string line = "";
		while (getline(programStream, line))
			shaderCode += "\n" + line;
		programStream.close();
	}
	else {
		cerr << "Error: Couldn't open " << shaderTypeString << " program '" << path << "'" << endl;
		exit(-1);
	}

	GLuint shaderRef = glCreateShader(shaderType == VertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	const char* shaderCodeCstring = shaderCode.c_str();
	glShaderSource(shaderRef, 1, &shaderCodeCstring, NULL);
	glCompileShader(shaderRef);

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(shaderRef, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shaderRef, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::" << shaderTypeString << "::COMPILATION_FAILED\n" << infoLog << endl;
		exit(-1);
	}
	return shaderRef;
}
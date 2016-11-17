#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"

class Shader {
public:
	enum Type { VertexShader, FragmentShader };
protected:
	GLuint programID;
public:
	Shader();
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	void deleteShader() { if(programID != 0) glDeleteProgram(programID); }
	GLuint getRef() { return programID; }
	void use() { glUseProgram(programID); }
protected:
	static GLuint compileShader(Type type, std::string path);
};

#endif
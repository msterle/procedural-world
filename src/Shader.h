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
	Shader() : programID(0) {};
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	void release() { 
		glDeleteProgram(programID);
		programID = 0;
	}
	~Shader() { release(); }
	GLuint getRef() { return programID; }
	void use() { glUseProgram(programID); }
protected:
	static GLuint compileShader(Type type, std::string path);
};

#endif
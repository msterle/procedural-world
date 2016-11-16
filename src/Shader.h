#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "../include/glew.h"
#include "../include/glfw3.h"

class Shader {
public:
	Shader();
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	GLuint getProgramRef();
	void use() { glUseProgram(this->programRef); }
protected:
	enum Type { VertexShader, FragmentShader };
	GLuint programRef;
	static GLuint compileShader(Type type, std::string path);
};

#endif
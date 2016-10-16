#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "../include/glfw3.h"

class ShaderProgram {
public:
	ShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
	GLuint getProgramRef();
protected:
	enum Type { VertexShader, FragmentShader };
	GLuint programRef;
	static GLuint compileShader(Type type, std::string path);
};

#endif /* SHADER_H */
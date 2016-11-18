#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "FrameBuffer.h"
#include "Texture.h"

class Texture;

class Filter {
protected:
	std::vector<float> kernel;
	std::vector<glm::vec2> offsets;
	unsigned int kernelSize;
	unsigned int scale;
	GLuint vao, vbo;
	GLsizei width, height;
	Shader* shader;
	FrameBuffer* fb;
public:
	Filter() { }
	Filter(std::vector<float> kernel, unsigned int scale = 1);
	~Filter();
	void apply(Texture* inTex, Texture* outTex);
protected:
	void initQuad();
};

#endif
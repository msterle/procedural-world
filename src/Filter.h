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
	GLuint vao, vbo;
	GLsizei width, height;
	Shader* shader;
	FrameBuffer* fb;
	DoubleBuffer* db;
	Texture* frontTex, * backTex;
	const GLenum buffers[2] = {GL_FRONT_LEFT, GL_BACK_LEFT};
	Texture* textures[2];
	int inBufferIndex;
public:
	Filter() { }
	Filter(std::vector<float> kernel);
	~Filter();
	void apply(Texture* inTex, Texture* outTex);
	void bind(Texture* inTex, Texture* outTex);
	void run();
	Texture* getOutTexture() { return textures[inBufferIndex]; }

protected:
	void initQuad();
};

#endif
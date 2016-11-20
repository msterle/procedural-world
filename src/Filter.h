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

// convolution filter
class Filter {
protected:
	std::vector<float> kernel;
	std::vector<glm::vec2> offsets, scaledOffsets;
	unsigned int kernelSize;
	GLuint vao, vbo;
	Shader* shader;
	FrameBuffer* fbo;
	Texture2D* inTex, * outTex;
	glm::vec2 texSize;
public:
	Filter() { }
	Filter(std::vector<float> kernel);
	~Filter();
	void apply(Texture2D* inTex, Texture2D* outTex);
	void bind(Texture2D* inTex, Texture2D* outTex);
	void run();
protected:
	void initQuad();
};


// seperable kernel convolution filter
class SeperableFilter : public Filter {
protected:
	Texture2D* interTex;
	bool ownInterTex;
public:
	SeperableFilter()  : interTex(NULL), ownInterTex(false) { }
	SeperableFilter(std::vector<float> kernel);
	void apply(Texture2D* inTex, Texture2D* outTex, Texture2D* interTex = NULL);
	void bind(Texture2D* inTex, Texture2D* outTex, Texture2D* interTex = NULL);
	void run();
	~SeperableFilter();
protected:
	void init(std::vector<float> kernel);
};


// gaussian blur convolution filter
class BlurFilter : public SeperableFilter {
public:
	BlurFilter(unsigned int size);
};

#endif
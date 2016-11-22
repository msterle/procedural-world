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

// Basic filter
class Filter {
	protected:
	GLuint vao, vbo;
	Shader* shader;
	FrameBuffer* fbo;
	glm::vec2 texSize;
public:
	Filter();
	~Filter();
protected:
	void initQuad();
};


// blender base class

class Blender : public Filter {
public:
	void multiply(Texture2D* inTex1, Texture2D* inTex2, Texture2D* outTex);
	void colorize(Texture2D* inTex, glm::vec3 colorDark, glm::vec3 colorLight, Texture2D* outTex);
	void colorize(Texture2D* inTex, glm::vec3 color, Texture2D* outTex) {
		colorize(inTex, glm::vec3(0), color, outTex);
	}
};


// linear convolution filter
class LinearFilter : public Filter {
protected:
	std::vector<float> kernel;
	std::vector<glm::vec2> offsets, scaledOffsets;
	unsigned int kernelSize;
	Texture2D* inTex, * outTex;
public:
	LinearFilter() { }
	LinearFilter(std::vector<float> kernel);
	void apply(Texture2D* inTex, Texture2D* outTex);
	void bind(Texture2D* inTex, Texture2D* outTex);
	void run();
};

class SorbelFilter : public LinearFilter {
public:
	SorbelFilter();
};


// seperable kernel convolution filter
class SeperableFilter : public LinearFilter {
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
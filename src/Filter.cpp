#include "Filter.h"

#include <iostream>
#include <cmath>
#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/type_ptr.hpp"
#include "Texture.h"
#include "FrameBuffer.h"
#include "helpers.h"

// debug only
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

Filter::Filter(vector<float> kernel) : kernel(kernel) {
	fbo = new FrameBuffer(true);
	// make kernel square with odd-numbered length of sides
	kernelSize = ceil((sqrt(kernel.size()) - 1) / 2) * 2 + 1;
	if(kernelSize * kernelSize != kernel.size()) {
		cerr << "Error: filter kernel not square, resizing to " 
			<< kernelSize << "x" << kernelSize << endl;
		kernel.resize(kernelSize * kernelSize, 0);
	}
	offsets.resize(kernelSize * kernelSize);
	scaledOffsets.resize(offsets.size());
	for(int x = 0; x < kernelSize; x++) {
		for(int y = 0; y < kernelSize; y++) {
			offsets[x + y * kernelSize] = glm::vec2((float)(x - ((int)kernelSize - 1) / 2), 
				(int)(y - ((float)kernelSize - 1) / 2));
		}
	}
	
	string shaderName = string("filter") + to_string(kernelSize) 
		+ string("x" + to_string(kernelSize) + ".frag");
	shader = new Shader(PathHelper::shader("filter.vert"), 
		PathHelper::shader(shaderName));

	initQuad();
	// bind global uniforms
	shader->use();
	glUniform1fv(glGetUniformLocation(shader->getRef(), "kernel"), kernelSize * kernelSize, 
		&kernel.front());
}

Filter::~Filter() {
	delete fbo;
	delete shader;
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

// TODO: const correctness
// TODO: convert to shared pointers

// render quad through filter and capture in frame buffer
void Filter::apply(Texture* inTex, Texture* outTex) {
	bind(inTex, outTex);
	run();
}

void Filter::bind(Texture* inTex, Texture* outTex) {
	this->inTex = inTex;
	this->outTex = outTex;
	fbo->attachTexture(outTex);
	shader->use();

	texSize = glm::vec2(outTex->getWidth(), outTex->getHeight());
	for(int i = 0; i < offsets.size(); ++i)
    	scaledOffsets[i] = offsets[i] / texSize;
	glUniform2fv(glGetUniformLocation(shader->getRef(), "offsets"), kernelSize * kernelSize, 
    	glm::value_ptr(*scaledOffsets.data()));
}

void Filter::run() {
	// save viewport dimensions
	shader->use();
	GLint viewportParams[4];
	glGetIntegerv(GL_VIEWPORT, viewportParams);

	// set viewport and bind fbo
	glViewport(0, 0, texSize.x, texSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->getRef());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// bind input texture and quad
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inTex->getRef());
	glBindVertexArray(vao);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(viewportParams[0], viewportParams[1], viewportParams[2], viewportParams[3]);
}


// Protected methods

void Filter::initQuad() {
	// set up quad
	GLfloat quadVertices[] = {
		// Positions         // Texture Coords
		-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
		1.0f,   1.0f, 0.0f,  1.0f, 1.0f,
		1.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
	};

	// set up VAO/VBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
		(GLvoid*)(3 * sizeof(GLfloat)));
}


/// Seperable Filter

SeperableFilter::SeperableFilter(vector<float> kernel) {
	init(kernel);
}

SeperableFilter::~SeperableFilter() {
	if(this->interTex != NULL && ownInterTex)
		delete interTex;
}

// TODO: const correctness
// TODO: convert to shared pointers

// render quad through filter and capture in frame buffer
void SeperableFilter::apply(Texture* inTex, Texture* outTex, Texture* interTex) {
	if(interTex == NULL) {
		ownInterTex = true;
		interTex = new Texture(outTex);
	}
	this->interTex = interTex;

	bind(inTex, outTex, interTex);
	run();

	if(ownInterTex) {
		delete interTex;
		ownInterTex = false;
	}
}

void SeperableFilter::bind(Texture* inTex, Texture* outTex, Texture* interTex) {
	// delete existing intermediate texture if have ownership
	if(this->interTex != NULL && ownInterTex) {
		delete interTex;
		ownInterTex = false;
	}
	// if no intermediate texture passed, create one
	if(interTex == NULL) {
		ownInterTex = true;
		interTex = new Texture(outTex);
	}
	this->interTex = interTex;
	this->inTex = inTex;
	this->outTex = outTex;
	fbo->attachTexture(vector<Texture*>{outTex, interTex});
	
	// set up shader
	shader->use();
	texSize = glm::vec2(outTex->getWidth(), outTex->getHeight());
	for(int i = 0; i < offsets.size(); ++i)
    	scaledOffsets[i] = offsets[i] / texSize;
	glUniform2fv(glGetUniformLocation(shader->getRef(), "offsets"), kernelSize, 
    	glm::value_ptr(*scaledOffsets.data()));
}

void SeperableFilter::run() {
	shader->use();
	// save viewport dimensions
	GLint viewportParams[4];
	glGetIntegerv(GL_VIEWPORT, viewportParams);

	// set viewport and bind fbo
	glViewport(0, 0, texSize.x, texSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->getRef());
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// bind quad
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	
	// draw horizontal pass to intermediate texture
	glBindTexture(GL_TEXTURE_2D, inTex->getRef());
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// draw vertical pass to output texture
	for(glm::vec2 &o : scaledOffsets)
    	o = glm::vec2(o.y, o.x);
	glUniform2fv(glGetUniformLocation(shader->getRef(), "offsets"), kernelSize, 
    	glm::value_ptr(*scaledOffsets.data()));
	glBindTexture(GL_TEXTURE_2D, interTex->getRef());
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	// unbind
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(viewportParams[0], viewportParams[1], viewportParams[2], viewportParams[3]);
}


// protected methods

void SeperableFilter::init(std::vector<float> kernel) {
	this->kernel = kernel;
	interTex = NULL;
	ownInterTex = false;
	// make kernel odd-numbered
	int size = kernel.size();
	kernelSize = ceil((size - 1) / 2) * 2 + 1;
	if(kernelSize != size) {
		cerr << "Error: filter kernel not odd, resizing to " 
			<< kernelSize << "x" << kernelSize << endl;
		kernel.resize(kernelSize, 0);
	}

	offsets.resize(kernelSize);
	for(int x = 0; x < kernelSize; x++)
		offsets[x] = glm::vec2((int)(x - ((int)kernelSize - 1) / 2), 0);
	scaledOffsets.resize(kernelSize);

	string shaderName = string("filter1x") + to_string(kernelSize) + string(".frag");
	shader = new Shader(PathHelper::shader("filter.vert"), 
		PathHelper::shader(shaderName));

	fbo = new FrameBuffer(true);

	initQuad();
	// bind global uniforms
	shader->use();
	glUniform1fv(glGetUniformLocation(shader->getRef(), "kernel"), kernelSize, 
		&kernel.front());
}


//// Blur Filter

BlurFilter::BlurFilter(unsigned int size) {
	// TODO: generate algorithmically
	float kernel3x3[] = {0.27901, 0.44198, 0.27901};
	float kernel5x5[] = {0.06136, 0.24477, 0.38774, 0.24477, 0.06136};
	float kernel7x7[] = {0.00598, 0.060626, 0.241843, 0.383103, 0.241843, 0.060626, 0.00598};
	
	switch(size) {
	case 3:
		kernel = vector<float>(kernel3x3, kernel3x3 + 3);
		break;
	case 5:
		kernel = vector<float>(kernel5x5, kernel5x5 + 5);
		break;
	case 7:
		kernel = vector<float>(kernel7x7, kernel7x7 + 7);
		break;
	default:
		cerr << "Invalid kernel size: " << kernelSize << endl;
		return;
	}

	init(kernel);
}
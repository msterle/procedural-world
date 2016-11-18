#include "Filter.h"

#include <iostream>
#include <cmath>
#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "Texture.h"
#include "FrameBuffer.h"
#include "helpers.h"

// debug only
#include "../include/glm/gtx/string_cast.hpp"

using namespace std;

Filter::Filter(vector<float> kernel, unsigned int scale) 
		: kernel(kernel), scale(scale) {
	fb = new FrameBuffer(true);
	// make kernel square with odd-numbered length of sides
	kernelSize = ceil((sqrt(kernel.size()) - 1) / 2) * 2 + 1;
	if(kernelSize * kernelSize != kernel.size()) {
		cerr << "Error: filter kernel not square, resizing to " 
			<< kernelSize << "x" << kernelSize << endl;
		kernel.resize(kernelSize * kernelSize, 0);
	}
	offsets.resize(kernelSize * kernelSize);
	for(int x = 0; x < kernelSize; x++) {
		for(int y = 0; y < kernelSize; y++) {
			offsets[x + y * kernelSize] = glm::vec2((int)scale * (x - ((int)kernelSize - 1) / 2), 
				(int)scale * (y - ((int)kernelSize - 1) / 2));
		}
	}
	switch(kernelSize) {
	case 3:
		shader = new Shader(PathHelper::shader("filter.vert"), 
			PathHelper::shader("filter3x3.frag"));
	}
	initQuad();
	
}

Filter::~Filter() {
	delete fb;
	delete shader;
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

// TODO: const correctness
// TODO: convert to shared pointers

// render quad through filter and capture in frame buffer
void Filter::apply(Texture* inTex, Texture* outTex) {
	fb->attachTexture(outTex);
	shader->use();
	
	// save viewport dimensions
	GLint viewportParams[4];
	glGetIntegerv(GL_VIEWPORT, viewportParams);

	// set viewport and bind fb
	glViewport(0, 0, outTex->getWidth(), outTex->getHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, fb->getRef());
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
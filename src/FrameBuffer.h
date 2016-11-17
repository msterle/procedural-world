#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Texture.h"

class FrameBuffer {
public:
	enum Type {COLOR, DEPTH, COLOR_DEPTH};
protected:
	GLuint fbo;
	Type type;
public:
	FrameBuffer() : fbo(0) { }
	FrameBuffer(const Texture& tex);
	FrameBuffer(const Texture& colorTex, const Texture& depthTex);
	void deleteFrameBuffer() { if(fbo != 0) glDeleteFramebuffers(1, &fbo); }
	GLuint getRef() const { return fbo; }
};

#endif
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
	// allow fallback to default constructor behaviour
	FrameBuffer(bool newFBO = false) : fbo(0) { if(newFBO) glGenFramebuffers(1, &fbo); }
	FrameBuffer(Texture* tex);
	FrameBuffer(Texture* colorTex, Texture* depthTex);
	void release() {
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	~FrameBuffer() { release(); }
	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
	void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	void attachTexture(Texture* tex);
	void attachTexture(Texture* colorTex, Texture* depthTex);
	GLuint getRef() const { return fbo; }
};

class DoubleBuffer : public FrameBuffer {
public:
	DoubleBuffer(bool newFBO = false) : FrameBuffer(newFBO) { }
	DoubleBuffer(Texture* frontTex, Texture* backTex);
	void attachTexture(Texture* frontTex, Texture* backTex);
};

#endif
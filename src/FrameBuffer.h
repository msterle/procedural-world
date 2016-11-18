#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
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
	FrameBuffer(const std::vector<Texture*>& texs);
	void release() {
		glDeleteFramebuffers(1, &fbo);
		fbo = 0;
	}
	~FrameBuffer() { release(); }
	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
	void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	void attachTexture(Texture* tex, unsigned int layer = 0);
	void attachTexture(const std::vector<Texture*>& texs);
	GLuint getRef() const { return fbo; }
};

#endif
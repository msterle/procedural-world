#include "FrameBuffer.h"

#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(Texture* tex) : fbo(0) {
	attachTexture(tex);
}

FrameBuffer::FrameBuffer(Texture* colorTex, Texture* depthTex) : fbo(0) {
	attachTexture(colorTex, depthTex);
}

void FrameBuffer::attachTexture(Texture* tex) {
	std::cout << "tex.ref: " << tex->getRef() << std::endl;
	if(fbo == 0)
		glGenFramebuffers(1, &fbo);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(tex->getFormat() == GL_DEPTH_COMPONENT) {
		type = DEPTH;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
			tex->getRef(), 0);
	}
	else {
		type = COLOR;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
			tex->getRef(), 0);
	}
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "newFrameBuffer error " << status << std::endl;
		fbo = 0;
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attachTexture(Texture* colorTex, Texture* depthTex) {
	if(fbo == 0)
		glGenFramebuffers(1, &fbo);
	type = COLOR_DEPTH;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
		colorTex->getRef(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
		depthTex->getRef(), 0);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "newFrameBuffer error " << status << std::endl;
		fbo = 0;
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}
	
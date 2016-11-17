#include "FrameBuffer.h"

#include <iostream>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(const Texture& tex) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(tex.getFormat() == GL_DEPTH_COMPONENT) {
		type = DEPTH;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
			tex.getRef(), 0);
	}
	else {
		type = COLOR;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
			tex.getRef(), 0);
	}
	
	if(GLenum e = glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "newFrameBuffer error " << e << std::endl;
		fbo = 0;
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(const Texture& colorTex, const Texture& depthTex) {
	type = COLOR_DEPTH;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
		colorTex.getRef(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
		depthTex.getRef(), 0);
	
	if(GLenum e = glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "newFrameBuffer error " << e << std::endl;
		fbo = 0;
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}
#include "FrameBuffer.h"

#include <iostream>
#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"
#include "Texture.h"

using namespace std;

FrameBuffer::FrameBuffer(Texture* tex) : fbo(0) {
	attachTexture(tex);
}

FrameBuffer::FrameBuffer(const vector<Texture*>& texs){
	for(int i = 0; i < texs.size(); ++i)
		attachTexture(texs[i], i);
}

void FrameBuffer::attachTexture(Texture* tex, unsigned int layer) {
	if(fbo == 0)
		glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if(tex->getFormat() == GL_DEPTH_COMPONENT) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 
			tex->getRef(), 0);
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + layer, GL_TEXTURE_2D, 
			tex->getRef(), 0);
	}
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "newFrameBuffer error " << status << endl;
		fbo = 0;
	}

	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attachTexture(const std::vector<Texture*>& texs) {
	for(int i = 0; i < texs.size(); ++i)
		attachTexture(texs[i], i);
}
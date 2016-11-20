#include "Texture.h"

#include "../include/glew.h"
#include "../include/glfw3.h"

// debug only
#include <iostream>
using namespace std;

////// Texture class
//// public methods

void Texture::setWrap(GLenum wrap) {
	this->wrap = wrap;
	bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	unbind();
}

void Texture::setBorder(Border border) {
	this->border = border;
	bind();
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border.components);
	unbind();
}

void Texture::setFilterMode(FilterMode filter) {
	this->filter = filter;
	bind();
	switch(filter) {
	case NEAREST:
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case LINEAR:
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case MIPMAP:
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(target);
		break;
	}
	unbind();
}

//// protected methods

void Texture::init(const GLvoid* pixels) {
	if(texID != 0)
		return;

	glGenTextures(1, &texID);
	
	createImage(pixels);

	setWrap(wrap);
	setBorder(border);
	setFilterMode(filter);
}


////// Texture2D class
//// protected methods

void Texture1D::createImage(const GLvoid* pixels) {
	bind();
	glTexImage1D(target, 0, internalFormat, dims[0], 0, format, type, pixels);
	unbind();
}

void Texture1D::updateImage(const GLvoid* pixels, Dimensions dims, Offset off) {
	bind();

	glTexSubImage1D(target, 0, off[0], dims[0], format, type, pixels);
	unbind();
}


////// Texture2D class
//// protected methods

void Texture2D::createImage(const GLvoid* pixels) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, dims[0]);
	bind();
	glTexImage2D(target, 0, internalFormat, dims[0], dims[1], 0, format, type, pixels);
	unbind();
}

void Texture2D::updateImage(const GLvoid* pixels, Dimensions dims, Offset off) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, this->dims[0]);
	bind();
	glTexSubImage2D(target, 0, off[0], off[1], dims[0], dims[1], format, type, pixels);
	unbind();
}
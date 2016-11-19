#include "Texture.h"

#include "../include/glew.h"
#include "../include/glfw3.h"


//// constructors

Texture::Texture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, 
		GLenum type, GLenum wrap, Border border, FilterMode filter, const GLvoid* data)
			: internalFormat(internalFormat), width(width), height(height), format(format),
			type(type) {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

	setWrap(wrap);
	setBorder(border);
	setFilterMode(filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}


//// public methods
// TODO: look into glTexStorage?

void Texture::writePixelData(const GLvoid* data) {
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);
}

void Texture::writePixelData(GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, 
		const GLvoid* data) {
	glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format, type, data);
}

void Texture::setWrap(GLenum wrap) {
	this->wrap = wrap;
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setBorder(Border border) {
	this->border = border;
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border.components);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setBorder(float color) {
	border = Border(color);
	setBorder(border);
}

void Texture::setFilterMode(FilterMode filter) {
	this->filter = filter;
	glBindTexture(GL_TEXTURE_2D, texID);
	switch(filter) {
	case NEAREST:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	case LINEAR:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case MIPMAP:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		break;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
#ifndef TEXTURE_H
#define TEXTURE_H

#include "../include/glew.h"
#include "../include/glfw3.h"

class Texture {
public:
	struct Border {
		GLfloat components[4];
		Border(GLfloat c) : components{c, c, c, c} { }
		Border() : Border(0) { }
	};
	enum FilterMode {NEAREST, LINEAR, MIPMAP};
protected:
	GLuint texID;
	GLint internalFormat;
	GLsizei width, height;
	GLenum format, type, wrap;
	Border border;
	FilterMode filter;
public:
	Texture() : texID(0) { }
	Texture(GLint internalFormat, GLsizei width, GLsizei height, 
			GLenum format, GLenum type, GLenum wrap = GL_REPEAT, 
			Border border = Border(0), FilterMode filter = NEAREST);
	void deleteTexture() { if(texID != 0) glDeleteTextures(1, &texID); }
	void setWrap(GLenum wrap);
	void setBorder(Border border);
	void setBorder(float color);
	void setFilterMode(FilterMode filter);
	GLuint getRef() const { return texID; }
	GLint getFormat() const { return format; }
};

#endif
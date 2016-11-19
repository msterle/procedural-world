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
	Texture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* data = NULL);
	Texture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, 
			const GLvoid* data = NULL) 
			: Texture(internalFormat, width, height, format, type, GL_REPEAT, Border(0), 
				NEAREST, data) { }
	Texture(const Texture& orig) 
			: Texture(orig.internalFormat, orig.width, orig.height, 
				orig.format, orig.type, orig.wrap, orig.border, orig.filter) { }
	Texture(Texture* orig) 
			: Texture(orig->internalFormat, orig->width, orig->height, orig->format, orig->type, 
				orig->wrap, orig->border, orig->filter) { }
	void release() { 
		glDeleteTextures(1, &texID);
		texID = 0;
	}
	~Texture() { release(); }
	void writePixelData(const GLvoid* data);
	void writePixelData(GLint xoffset, GLint yoffset, GLsizei width, 
			GLsizei height, const GLvoid* data);
	void setWrap(GLenum wrap);
	void setBorder(Border border);
	void setBorder(float color);
	void setFilterMode(FilterMode filter);
	GLuint getRef() const { return texID; }
	GLint getFormat() const { return format; }
	GLsizei getWidth() const { return width; }
	GLsizei getHeight() const { return height; }
};

#endif
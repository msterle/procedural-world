#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "../include/glew.h"
#include "../include/glfw3.h"

// TODO: look into glPixelStore for pixel format params
// TODO: look into glTexStorage for faster but immutable storage
// TODO: look into copying texture pixels - glGetTexImage​, glCopyTexImage2D

// Abstract base class with common functionality
//
// Because we can't call overriden virtual methods from constructor, derived classes must call 
// init() in constructors.
class Texture {
public:
	struct Border {
		GLfloat components[4];
		Border(GLfloat c) : components{c, c, c, c} { }
		Border() : Border(0) { }
	};
	enum FilterMode {NEAREST, LINEAR, MIPMAP};
	typedef std::vector<GLsizei> Dimensions;
	typedef std::vector<GLint> Offset;
protected:
	GLuint texID;
	GLenum target, format, type, wrap;
	GLint internalFormat;
	Dimensions dims;
	Border border;
	FilterMode filter;
	// called by subclasses constructors for access to virtual methods
	virtual void init(const GLvoid* pixels = NULL) final;
	// pure virtual methods have to be implemented by subclasses
	virtual void createImage(const GLvoid* pixels) = 0;
	virtual void updateImage(const GLvoid* pixels, Dimensions dims, Offset off) = 0;
	// TODO: implement isDimCountCorrect() to check vector size against target
public:
	Texture(GLenum target, GLint internalFormat, Dimensions dims, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* pixels = NULL)
			: texID(0), target(target), internalFormat(internalFormat), dims(dims), format(format),
			type(type), wrap(wrap), border(border), filter(filter) { }
	Texture(GLenum target, const Texture& orig) 
			: Texture(target, orig.internalFormat, orig.dims, orig.format, orig.type, orig.wrap, 
				orig.border, orig.filter) { }
	//
	void release() { 
		glDeleteTextures(1, &texID);
		texID = 0;
	}
	~Texture() { release(); }
	// getters
	GLuint getRef()            const { return texID; }
	GLenum getTarget()         const { return target; }
	GLint getFormat()          const { return format; }
	Dimensions getDimensions() const { return dims; }
	GLsizei getWidth()         const { return dims[0]; }
	// setters
	void setWrap(GLenum wrap);
	void setBorder(Border border);
	void setBorder(float color) { setBorder(Border(color)); }
	void setFilterMode(FilterMode filter);
	// bind/unbind
	void bind() { glBindTexture(target, texID); }
	void unbind() { glBindTexture(target, 0); }
	// write method
	void setPixelData(const GLvoid* pixels, Dimensions dims = Dimensions(), 
			Offset off = Offset{0, 0}) {
		updateImage(pixels, dims, off);
	}
};


// Subclass for 1D textures
class Texture1D : public Texture {
protected:
	void createImage(const GLvoid* pixels = NULL) override;
	void updateImage(const GLvoid* pixels, Dimensions dims, Offset off) override;
public:
	// full constructor with combined dimension parameters
	Texture1D(GLint internalFormat, Dimensions dims, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* pixels = NULL)
			: Texture(GL_TEXTURE_1D, internalFormat, dims, format, type, wrap, border, filter, 
				pixels) { init(pixels); }
	// full constructor with separate dimension parameters
	Texture1D(GLint internalFormat, GLsizei width, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* pixels = NULL)
			: Texture1D(internalFormat, Dimensions{width}, format, type, wrap, border, 
				filter, pixels) { }
	// short pixel data constructor with combined dimension parameters
	Texture1D(GLint internalFormat, Dimensions dims, GLenum format, GLenum type, 
			const GLvoid* pixels = NULL) 
			: Texture1D(internalFormat, dims, format, type, GL_REPEAT, Border(0), NEAREST, 
				pixels) { }
	// short pixel data constructor with separate dimension parameters
	Texture1D(GLint internalFormat, GLsizei width, GLenum format, GLenum type, 
			const GLvoid* pixels = NULL) 
			: Texture1D(internalFormat, Dimensions{width}, format, type, GL_REPEAT, 
				Border(0), NEAREST, pixels) { }
	// copy constructor
	Texture1D(const Texture1D& orig) 
			: Texture(GL_TEXTURE_1D, orig) { init(); }
	// getters
	GLsizei getHeight() const { return dims[1]; }
};


// Subclass for 2D textures
class Texture2D : public Texture {
protected:
	void createImage(const GLvoid* pixels = NULL) override;
	void updateImage(const GLvoid* pixels, Dimensions dims, Offset off) override;
public:
	// full constructor with combined dimension parameters
	Texture2D(GLint internalFormat, Dimensions dims, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* pixels = NULL)
			: Texture(GL_TEXTURE_2D, internalFormat, dims, format, type, wrap, border, filter, 
				pixels) { init(pixels); }
	// full constructor with separate dimension parameters
	Texture2D(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, 
			GLenum wrap = GL_REPEAT, Border border = Border(0), FilterMode filter = NEAREST, 
			const GLvoid* pixels = NULL)
			: Texture2D(internalFormat, Dimensions{width, height}, format, type, wrap, border, 
				filter, pixels) { }
	// short pixel data constructor with combined dimension parameters
	Texture2D(GLint internalFormat, Dimensions dims, GLenum format, GLenum type, 
			const GLvoid* pixels = NULL) 
			: Texture2D(internalFormat, dims, format, type, GL_REPEAT, Border(0), NEAREST, 
				pixels) { }
	// short pixel data constructor with separate dimension parameters
	Texture2D(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, 
			const GLvoid* pixels = NULL) 
			: Texture2D(internalFormat, Dimensions{width, height}, format, type, GL_REPEAT, 
				Border(0), NEAREST, pixels) { }
	// copy constructor
	Texture2D(const Texture2D& orig) 
			: Texture(GL_TEXTURE_2D, orig) { init(); }
	// getters
	GLsizei getHeight() const { return dims[1]; }
};

#endif
#include "Texture.h"

#include "../include/glew.h"
#include "../include/glfw3.h"
#include <functional>

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
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border.components.data());
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


////// Texture1D class
//// protected methods

void Texture1D::createImage(const GLvoid* pixels) {
	bind();
	glTexImage1D(target, 0, internalFormat, dims[0], 0, format, type, pixels);
	unbind();
}

void Texture1D::updateImage(const GLvoid* pixels, Dimensions dims, Offset off) {
	if(dims.size() != 1)
		dims = this->dims;
	if(off.size() != 1)
		off = Offset(1, 0);
	bind();
	glTexSubImage1D(target, 0, off[0], dims[0], format, type, pixels);
	unbind();
}


////// Texture2D class
//// constructors

// generator methods

Texture2D::Texture2D(GLsizei width, GLsizei height, GeneratorRGBA8U generator)
		: Texture2D(GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE) {
	unsigned char* data = new unsigned char[width * height * 4];
	PixelRGBA8U pixel;
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			pixel = generator((float)x / width, (float)y / width);
			data[(x + y * width) * 4]     = pixel[0];
			data[(x + y * width) * 4 + 1] = pixel[1];
			data[(x + y * width) * 4 + 2] = pixel[2];
			data[(x + y * width) * 4 + 3] = pixel[3];
		}
	}
	updateImage(data);
	delete[] data;
}

Texture2D::Texture2D(GLsizei width, GLsizei height, GeneratorRGBA32F generator)
		: Texture2D(GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT) {
	float* data = new float[width * height * 4];
	PixelRGBA32F pixel;
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			pixel = {generator((float)x / width, (float)y / width)};
			data[(x + y * width) * 4]     = pixel[0];
			data[(x + y * width) * 4 + 1] = pixel[1];
			data[(x + y * width) * 4 + 2] = pixel[2];
			data[(x + y * width) * 4 + 3] = pixel[3];
		}
	}
	updateImage(data);
	delete[] data;
}

//// protected methods

void Texture2D::createImage(const GLvoid* pixels) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, dims[0]);
	bind();
	glTexImage2D(target, 0, internalFormat, dims[0], dims[1], 0, format, type, pixels);
	unbind();
}

void Texture2D::updateImage(const GLvoid* pixels, Dimensions dims, Offset off) {
	if(dims.size() != 2)
		dims = this->dims;
	if(off.size() != 2)
		off = Offset(3, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, this->dims[0]);
	bind();
	glTexSubImage2D(target, 0, off[0], off[1], dims[0], dims[1], format, type, pixels);
	unbind();
}


////// TextureCubemap class
//// constructors
// generator methods

TextureCubemap::TextureCubemap(GLsizei width, GLsizei height, Generator3DRGBA8U generator)
		: TextureCubemap(GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE) {
	unsigned char* data = new unsigned char[width * height * 4];
	PixelRGBA8U pixel;
	
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			pixel = generator((float)x / width - 0.5, (float)y / height, 1);
			data[(x + y * width) * 4]     = pixel[0];
			data[(x + y * width) * 4 + 1] = pixel[1];
			data[(x + y * width) * 4 + 2] = pixel[2];
			data[(x + y * width) * 4 + 3] = pixel[3];
		}
	}
	updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, data);

	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			pixel = generator((float)x / width - 0.5, (float)y / height, 1);
			data[(x + y * width) * 4]     = pixel[0];
			data[(x + y * width) * 4 + 1] = pixel[1];
			data[(x + y * width) * 4 + 2] = pixel[2];
			data[(x + y * width) * 4 + 3] = pixel[3];
		}
	}
	updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, data);
	
	delete[] data;
}

TextureCubemap::TextureCubemap(GLsizei width, GLsizei height, Generator3DRGBA32F generator)
		: TextureCubemap(GL_RGBA32F, width, height, GL_RGBA, GL_FLOAT) {
	float* data = new float[width * height * 4];
	PixelRGBA32F pixel;
	for(int x = 0; x < width; ++x) {
		for(int y = 0; y < height; ++y) {
			pixel = {generator((float)x / width, (float)y / width, (float)z / width)};
			data[(x + y * width) * 4]     = pixel[0];
			data[(x + y * width) * 4 + 1] = pixel[1];
			data[(x + y * width) * 4 + 2] = pixel[2];
			data[(x + y * width) * 4 + 3] = pixel[3];
		}
	}
	updateImage(data);
	delete[] data;
}


//// protected methods

void TextureCubemap::createImage(const GLvoid* pixels) {
	bind();
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, dims[0], dims[1], 0, format, type, NULL);
	unbind();
}

void TextureCubemap::updateImage(const GLvoid* pixels, Dimensions dims, Offset off) {
	if(dims.size() != 2)
		dims = this->dims;
	if(off.size() != 2)
		off = Offset(3, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, this->dims[0]);
	bind();
	glTexSubImage2D(target, 0, off[0], off[1], dims[0], dims[1], format, type, pixels);
	unbind();
}
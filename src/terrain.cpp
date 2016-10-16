#include "model.cpp"

#include "../include/CImg.h"
#include <vector>

// debugging only!
#include <iostream>

using namespace std;

class Terrain: public Model {
public:
	Terrain();
	void buildFromHeightmap(const cimg_library::CImg<unsigned char>& heightImg, const cimg_library::CImg<unsigned char>& ColorImg, int scale = 1);
	void draw();

private:
	GLuint colorBuffer;
	vector<glm::vec3> vertexColors;
	float upsample(int x, int y, int c, const cimg_library::CImg<unsigned char>& image, int width, int height, int scale);
};

Terrain::Terrain() {
	glGenBuffers(1, &this->colorBuffer);
}

void Terrain::buildFromHeightmap(const cimg_library::CImg<unsigned char>& heightImg, const cimg_library::CImg<unsigned char>& colorImg, int scale) {
	
	// input check
	if( heightImg.width() != colorImg.width() || heightImg.height() != colorImg.height() ) {
		cerr << "Error: input images not equal dimensions";
		exit(-1);
	}
	
	// build vertices
	this->vertices.clear();
	this->vertexColors.clear();
	int sourceWidth = heightImg.width(), sourceHeight = heightImg.width();
	int width = scale * sourceWidth;
	int height = scale * sourceHeight;
	float zMax = 0;
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			// For each pixel add a vertex with pixel's x and y values, z determined
			// from heightmap image's first channel value and color copied from color image
			glm::vec3 vertex;
			vertex.x = (float) x / (width - 1) - 0.5f;
			vertex.y = (float) y / (height - 1) - 0.5f;
			vertex.z = upsample(x, y, 0, heightImg, sourceWidth, sourceHeight, scale) / 255;
			if(vertex.z > zMax)
				zMax = vertex.z;
			this->vertices.push_back(vertex);
			
			glm::vec3 vertexColor;
			vertexColor.r = upsample(x, y, 0, colorImg, sourceWidth, sourceHeight, scale) / 255;
			vertexColor.g = upsample(x, y, 1, colorImg, sourceWidth, sourceHeight, scale) / 255;
			vertexColor.b = upsample(x, y, 2, colorImg, sourceWidth, sourceHeight, scale) / 255;
			this->vertexColors.push_back(vertexColor);
		}
	}

	// Center object on Z plane according to maximum z value
	for(std::vector<glm::vec3>::iterator it = this->vertices.begin(); it != this->vertices.end(); ++it) {
   		it->z -= zMax / 2;
	}

	// build indices
	this->indices.clear();
	for(int y = 0; y < height - 1; y++) {
		for(int x = 0; x < width; x++) {
			this->indices.push_back(x + y * width);
			this->indices.push_back(x + (y + 1) * width);
		}
		// Restart primitive at end of row
		if(y < height - 2) {
			this->indices.push_back(65535);
		}
	}

	this->scale(glm::vec3(1, 1, 0.5));

	// bind buffers
	glBindVertexArray(this->VAO);

	// bind vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices.front(), GL_STATIC_DRAW);
	GLint posAttrib = glGetAttribLocation(this->shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	// bind color buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->vertexColors.size() * sizeof(glm::vec3), &this->vertexColors.front(), GL_STATIC_DRAW);
	GLint colAttrib = glGetAttribLocation(this->shaderProgram, "in_colour");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// bind index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

float Terrain::upsample(int x, int y, int c, const cimg_library::CImg<unsigned char>& image, int width, int height, int scale) {
	// Weighted average of four surrounding source pixels
	// note: requires image width & height to reduce overhead of calling CImg methods
	// ex: in 4x4 upsampling (scale = 4), output pixel (1, 2) has weighted average of
	// surrounding 4 input pixels A (0,0), B (1, 0), C(0, 1) and D (1, 1)
	// 6/16 * A + 2/16 * B + 6/16 * C + 2/16 * D
	int xmod = x % scale, ymod = y % scale;
	int xLast = x / scale + 1 < width ? 1 : 0;
	int yLast = y / scale + 1 < height ? 1 : 0;
	// Optimized from operator() to
	return (  (float) *image.data(x / scale,         y / scale,         0, c) * (scale - xmod) * (scale - ymod) 
			+ (float) *image.data(x / scale + xLast, y / scale,         0, c) * xmod *           (scale - ymod) 
			+ (float) *image.data(x / scale,         y / scale + yLast, 0, c) * (scale - xmod) * ymod 
			+ (float) *image.data(x / scale + xLast, y / scale + yLast, 0, c) * xmod *           ymod ) 
		/ (scale * scale);     // faster than pow(scale, 2)
}

void Terrain::draw() {
	GLint loc_modelMat = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(this->modelMat));
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLE_STRIP, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include <cmath>
#include "../include/CImg.h"
#include "../include/glm/glm.hpp"
#include "Model.h"
#include "Vertex.h"

class Texture;

class Terrain: public Model {
protected:
	float width, length;
	float resolution;
	Mesh* mesh;
	MeshInstancePtr instancePtr;
	Texture2D* tex;
public:
	Terrain();
	//void buildFromHeightmap(std::string heightmapPath, std::string colorPath, int scale = 1);
	//void resampleHeightmap(int scale = 1);
	//void generateHills(int width, int number);
	void generateDiamondSquare(float width, float resolution, float roughness);
	void generatePlane(float width, float length);
	float getYAtXZWorld(float x, float z);
	glm::vec3 getNormalAtXZWorld(float x, float z);
	float getWidth() { return width; }
	float getLength() { return length; }
	float getResolution() { return resolution; }
	unsigned int getVerticesXCount() { return round(width * resolution); }
	unsigned int getVerticesZCount() { return round(length * resolution); }
protected:
	//GLuint colorBuffer;
	//cimg_library::CImg<unsigned char> heightImg, colorImg;
	//std::vector<glm::vec3> vertexColors;
	void recurseDiamondSquare(std::vector<Vertex>* vertices, int width, int size, float roughness);
	void generateNormals(std::vector<Vertex>* vertices, int width, int height);
};

#endif
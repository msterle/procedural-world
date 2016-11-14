#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include "../include/CImg.h"
#include "../include/glm/glm.hpp"
#include "Model.h"
#include "Vertex.h"

class Terrain: public Model {
public:
	Terrain();
	//void buildFromHeightmap(std::string heightmapPath, std::string colorPath, int scale = 1);
	//void resampleHeightmap(int scale = 1);
	//void generateHills(int width, int number);
	void generateDiamondSquare(int aproxWidth, float roughness);
	void generatePlane(float width, float length);
	float getYAtXZWorld(float x, float z);
	float getWidth() { return width; }
	float getLength() { return length; }
protected:
	float width, length;
	Mesh* mesh;
	MeshInstancePtr instancePtr;
	//GLuint colorBuffer;
	//cimg_library::CImg<unsigned char> heightImg, colorImg;
	//std::vector<glm::vec3> vertexColors;
	void recurseDiamondSquare(std::vector<Vertex>* vertices, int width, int size, float roughness);
	void generateNormals(std::vector<Vertex>* vertices, int width, int height);
};

#endif
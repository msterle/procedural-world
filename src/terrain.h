#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <vector>
#include "../include/CImg.h"
#include "../include/glm/glm.hpp"
#include "model.h"

class Terrain: public Model {
public:
	Terrain();
	void buildFromHeightmap(std::string heightmapPath, std::string colorPath, int scale = 1);
	void resampleHeightmap(int scale = 1);
	void generateHills(int width, int number);
	void generateDiamondSquare(int aproxWidth, float roughness);
	void draw();
protected:
	GLuint colorBuffer;
	cimg_library::CImg<unsigned char> heightImg, colorImg;
	std::vector<glm::vec3> vertexColors;
	float upsample(int x, int y, int c, const cimg_library::CImg<unsigned char>& image, int width, int height, int scale);
	void subDiamondSquare(int width, int size, float roughness);
	std::vector<glm::vec3> getNormals(int width, int height);
};

#endif
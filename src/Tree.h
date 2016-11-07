#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>

#include "MeshInstance.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

using namespace std;


class Tree {
public:
	Tree();
	void draw(Shader shader);
private:
	Mesh* mesh;
	bool loadOBJ(const char * path);
	glm::mat4 modelMat;
	Material* material;
};

#endif
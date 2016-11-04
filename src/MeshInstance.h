#ifndef MESHINSTANCE_H
#define MESHINSTANCE_H

#include <vector>
#include "../include/glm/gtc/type_ptr.hpp"

class Mesh;
struct Material;

class MeshInstance {
public:
	MeshInstance(Mesh* mesh, Material* material);
	void translate(glm::vec3 t);
	void scale(glm::vec3 s);
	void rotate(float angle, glm::vec3 axis);
private:
	Mesh* mesh;
	Material* material;
	glm::mat4 instanceMat;	
};

#endif
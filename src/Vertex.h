#ifndef VERTEX_H
#define VERTEX_H

#include "../include/glm/gtc/type_ptr.hpp"

struct Vertex {
	glm::vec3 position, normal;
	//Vertex(glm::vec3 position, glm::vec3 normal) : position(position), normal(normal) {}
};

#endif
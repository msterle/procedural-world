#ifndef SKYBOX_H
#define SKYBOX_H

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include <vector>
#include <cmath>

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

class Skybox {
protected:
	GLuint VAO, VBO;
	std::vector<glm::vec3> vertices;
	TextureCubemap* tex;
	Shader* shader;
public:
	Skybox(float size = 100.0f);
	~Skybox();
	void draw(glm::mat4 view, glm::mat4 projection);
};

#endif
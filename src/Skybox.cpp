#include "Skybox.h"

#include "../include/glew.h"
#include "../include/glfw3.h"
#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <vector>
#include <iostream>

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"
#include "helpers.h"
#include "PerlinNoise.h"

using namespace std;

Skybox::Skybox() : tex(NULL) {
	shader = new Shader(PathHelper::shader("skybox.vert"),
		PathHelper::shader("skybox.frag"));

	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f,  20.0f));
	vertices.push_back(glm::vec3(-20.0f,  20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f, -20.0f));
	vertices.push_back(glm::vec3(-20.0f, -20.0f,  20.0f));
	vertices.push_back(glm::vec3( 20.0f, -20.0f,  20.0f));

	// Generate VAO, VBO, EBO, IBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindVertexArray(0);

	PerlinNoise pnoise(237);
	tex = new TextureCubemap(400, 400, [pnoise](float x, float y, float z)->Texture::PixelRGBA32F {
		float val = floor(10.0 * pnoise.octaveNoise(5.0 * x, 5.0 * y, 5.0 * z, 2, 0.5)) / 10.0;
		//float val = (-y / 2 + 0.5) * 2;
		return {val, val, val, 1.0};
	});
	tex->setFilterMode(Texture::LINEAR);
}

Skybox::~Skybox() {
	delete shader;
}

void Skybox::draw(glm::mat4 view, glm::mat4 projection) {

	shader->use();
	
	glUniformMatrix4fv(glGetUniformLocation(shader->getRef(), "view"), 1, GL_FALSE, 
		glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader->getRef(), "projection"), 1, GL_FALSE, 
		glm::value_ptr(projection));

	// bind texture if it exists
	if(tex != NULL) {
		glUniform1i(glGetUniformLocation(shader->getRef(), "skyboxTex"), 0);
	    glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex->getRef());
	}

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	
	glBindVertexArray(0);
}
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

Skybox::Skybox(float size) : tex(NULL) {
	shader = new Shader(PathHelper::shader("skybox.vert"),
		PathHelper::shader("skybox.frag"));

	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3(-size, -size, -size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3( size,  size, -size));
	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3(-size, -size, -size));
	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3(-size,  size,  size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3( size, -size,  size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3( size,  size, -size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3(-size,  size,  size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3( size, -size,  size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3( size,  size, -size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3( size,  size,  size));
	vertices.push_back(glm::vec3(-size,  size,  size));
	vertices.push_back(glm::vec3(-size,  size, -size));
	vertices.push_back(glm::vec3(-size, -size, -size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3( size, -size, -size));
	vertices.push_back(glm::vec3(-size, -size,  size));
	vertices.push_back(glm::vec3( size, -size,  size));

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
	Texture::PixelRGBA32F colorDark = {0.62, 0.82, 1};
	Texture::PixelRGBA32F colorLight = {1, 1, 1, 1};

	tex = new TextureCubemap(400, 400, [pnoise, colorDark, colorLight](float x, float y, float z)->Texture::PixelRGBA32F {
		// speed things up by ignoring values where y < 0
		if(y < 0)
			return {1, 1, 1, 1};
		float val = min(1.0, 
			  max(0.0, pnoise.octaveNoise(15.0 * x / y, 1.0, 15.0 * z / y, 8, 0.5) - 0.5) * 2.0
			* max(0.0, pnoise.octaveNoise(5.0 * x / y, 1.0, 5.0 * z / y, 2, 0.5) - 0.4)
			* 10.0 / 6.0 * 3
			+ pow(100.0, -y)); // add for raleigh scattering
		Texture::PixelRGBA32F pixel = {
			fma(val, colorLight[0], fma(-val, colorDark[0], colorDark[0])),
			fma(val, colorLight[1], fma(-val, colorDark[1], colorDark[1])),
			fma(val, colorLight[2], fma(-val, colorDark[2], colorDark[2])),
			fma(val, colorLight[3], fma(-val, colorDark[3], colorDark[3]))
		};
		return pixel;
	});

	tex->setFilterMode(Texture::NEAREST);
}

Skybox::~Skybox() {
	delete shader;
	delete tex;
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
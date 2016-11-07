#include "ModelManager.h"

#include <string>
#include <vector>
#include <algorithm>

#include "ModelNew.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "Material.h"
#include "Shader.h"

// debugging only
#include <iostream>

using namespace std;


//// Public methods

// Factory methods for Model

ModelNew* ModelManager::newModel(){
	ModelNew model;
	model.setIndex(this->models.size());
	this->models.push_back(model);
	return &(this->models.back());
}

void ModelManager::deleteModel(unsigned int deleteModelIndex) {
	// remove vector element
	this->models.erase(this->models.begin() + deleteModelIndex);

	// decrement modelIndex for each model after delete point
	for(std::vector<ModelNew>::iterator itModel = this->models.begin() + deleteModelIndex; itModel != this->models.end(); ++itModel) {
		unsigned int modelIndex = itModel->getIndex();
		if(modelIndex > deleteModelIndex) {
			itModel->setIndex(modelIndex - 1);
		}
	}

	// remove instances for each Mesh
	for(std::vector<Mesh>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
		it->removeModel(deleteModelIndex);
	}
}

// Factory methods for Mesh

Mesh* ModelManager::newMesh(vector<Vertex> vertices) {
	Mesh mesh(vertices);
	this->meshes.push_back(mesh);
	return &(this->meshes.back());
}

Mesh* ModelManager::newMesh(vector<Vertex> vertices, vector<GLuint> indices) {
	Mesh mesh(vertices, indices);
	this->meshes.push_back(mesh);
	return &(this->meshes.back());
}

Mesh* ModelManager::newMesh(const string& filePath) {
	Mesh mesh(filePath);
	this->meshes.push_back(mesh);
	return &(this->meshes.back());
}

// Factory method for MeshInstance

MeshInstance* ModelManager::newMeshInstance(unsigned int modelIndex, unsigned int meshId, Material material) {
	vector<Mesh>::iterator it = find_if(this->meshes.begin(), this->meshes.end(), 
		[&meshId](const Mesh& mesh) {
			return mesh.getId() == meshId;
		});
	MeshInstance* instance = it->newInstance(modelIndex, material);
	return instance;
}

MeshInstance* ModelManager::newMeshInstance(MeshInstance* original) {
	// need to copy because 'original' becomes invalid after MeshInstance created
	glm::mat4 temp = original->instanceMat;
	MeshInstance* instance = newMeshInstance(original->modelIndex, original->meshId, original->material);
	instance->instanceMat = temp;
	return instance;
}

// Draw method

void ModelManager::draw(Shader shader) {
	GLuint shaderProgram = shader.getProgramRef();

	const int MY_ARRAY_SIZE = 512;
	GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, "modelMatsBlock");
	glUniformBlockBinding(shaderProgram, blockIndex, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof (ModelNew) * MY_ARRAY_SIZE, &this->models.front(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, this->UBO);
	
	// uniform model matrix
	GLint loc_modelMat = glGetUniformLocation(shaderProgram, "modelMat");
	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

	// draw each mesh
	for(std::vector<Mesh>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
		it->draw(shader);
	}
}
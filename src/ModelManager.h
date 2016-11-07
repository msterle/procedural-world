#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <string>
#include <vector>

#include "../include/glew.h"
#include "../include/glfw3.h"

#include "ModelNew.h"
#include "Mesh.h"
#include "MeshInstance.h"

using namespace std;

// Singleton class for model, mesh and mesh instance management

class ModelManager {
public:
	static ModelManager& getInstance() {
		static ModelManager instance;
		return instance;
	}
    // factory methods
    ModelNew* newModel();
    Mesh* newMesh(vector<Vertex> vertices);
    void deleteModel(unsigned int modelIndex);
	Mesh* newMesh(vector<Vertex> vertices, vector<GLuint> indices);
	Mesh* newMesh(const string& filePath);
    MeshInstance* newMeshInstance(unsigned int modelIndex, unsigned int meshId, Material material);
    MeshInstance* newMeshInstance(MeshInstance* original);
    void removeMeshInstancesByModelIndex(unsigned int modelIndex);
    // other methods
    void draw(Shader shader);
protected:
	GLuint UBO;
	vector<Mesh> meshes;
	vector<ModelNew> models;
	ModelManager() { glGenBuffers(1, &this->UBO); }
public:
	// remove copy constructors
	//ModelManager(ModelManager const&) = delete;
    void operator=(ModelManager const&) = delete;
};

#endif
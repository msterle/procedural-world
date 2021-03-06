#include "ParaTree.h"

#include <string>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <list>
#include <functional>

#include "../include/glm/gtc/type_ptr.hpp"

#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "PLS.h"
#include "Turtle.h"
#include "helpers.h"
#include "World.h"
#include "Terrain.h"

using namespace std;
using namespace std::placeholders;

// debug only
#include <stdio.h>
#include <iostream>
#include "../include/glm/gtx/string_cast.hpp"


// Constructor

ParaTree::ParaTree(ParaTree::TreeParams treeParams, unsigned int n, Texture2D* branchTex, Texture2D* leafTex) 
	: branchTex(branchTex), leafTex(leafTex) {
	treePLS = PLS(
		// axiom
		{
			PLS::Module('A', vector<PLS::Param>{
				PLS::Param(100), 
				PLS::Param(treeParams.w0)})
		},
		// production rules
		{
			{
				'A',
				PLS::Production(
					// condition
					bind(PLS::gteq, _1, treeParams.min),
					// replacement
					{
						PLS::Module('!', 
							PLS::Param(bind(PLS::pass, _2))),
						PLS::Module('F', 
							PLS::Param(bind(PLS::pass, _1))),
						PLS::Module('['),
						PLS::Module('+', 
							PLS::Param(treeParams.a1)),
						PLS::Module('/', 
							PLS::Param(treeParams.f1)),
						PLS::Module('A', vector<PLS::Param>{
							PLS::Param(bind(PLS::mult, _1, treeParams.r1)), 
							PLS::Param(bind(PLS::mult, _2, pow(treeParams.q, treeParams.e)))}),
						PLS::Module(']'),
						PLS::Module('['),
						PLS::Module('+', 
							PLS::Param(treeParams.a2)),
						PLS::Module('/', 
							PLS::Param(treeParams.f2)),
						PLS::Module('A', vector<PLS::Param>{
							PLS::Param(bind(PLS::mult, _1, treeParams.r2)), 
							PLS::Param(bind(PLS::mult, _2, pow(1 - treeParams.q, treeParams.e)))}),
						PLS::Module(']')
					}
				)
			}
		},
		{
			{
				'!',
				bind(ParaTree::Actions::setWidth, (void*)this, _1)
			},
			{
				'F',
				bind(ParaTree::Actions::forward, (void*)this, _1)
			},
			{
				'A',
				bind(ParaTree::Actions::apex, (void*)this, _1, _2)
			},
			{
				'+',
				bind(ParaTree::Actions::turnLeft, (void*)this, _1)
			},
			{
				'-',
				bind(ParaTree::Actions::turnRight, (void*)this, _1)
			},
			{
				'/',
				bind(ParaTree::Actions::rollLeft, (void*)this, _1)
			},
			{
				'\\',
				bind(ParaTree::Actions::rollRight, (void*)this, _1)
			},
			{
				'[',
				bind(ParaTree::Actions::push, (void*)this)
			},
			{
				']',
				bind(ParaTree::Actions::pop, (void*)this)
			}
		}
	);

	generate(n);
}

// protected methods

void ParaTree::generate(unsigned int n) {
	string objPath = PathHelper::model("cylinder8tex.obj");
	cylinderMesh = newMesh(objPath, branchTex);

	vector<Vertex> vertices;
	vertices.push_back({
		glm::vec3(-1, 0, -1),
		glm::vec3(0, 1, 0),
		glm::vec2(0, 0)
	});
	vertices.push_back({
		glm::vec3(-1, 0, 1),
		glm::vec3(0, 1, 0),
		glm::vec2(0, 1)
	});
	vertices.push_back({
		glm::vec3(1, 0, 1),
		glm::vec3(0, 1, 0),
		glm::vec2(1, 1)
	});
	vertices.push_back({
		glm::vec3(-1, 0, -1),
		glm::vec3(0, 1, 0),
		glm::vec2(0, 0)
	});
	vertices.push_back({
		glm::vec3(1, 0, 1),
		glm::vec3(0, 1, 0),
		glm::vec2(1, 1)
	});
	vertices.push_back({
		glm::vec3(1, 0, -1),
		glm::vec3(0, 1, 0),
		glm::vec2(1, 0)
	});


	leafMesh = newMesh(vertices, GL_TRIANGLES, leafTex);

	// generate tree
	treePLS.iterate(n);

	// process modules
	treePLS.run();

}

// protected static functions

void ParaTree::Actions::forward(void* v_self, float length) {
	ParaTree* self = (ParaTree*)v_self;
	// advance turtle
	self->turtle.forward(length / 50);
	// place new segment behind
	MeshInstancePtr segment = self->cylinderMesh->newInstance(Materials::bark);
	segment->scale(glm::vec3(1, length / 50, 1)); // set height
	segment->rotate(glm::radians(90.0f), glm::vec3(1, 0, 0)); // rotate to facing +Z to match turtle start orientation
	segment->applyMatrix(self->turtle.getMatrix()); // transform according to turtle
}

void ParaTree::Actions::apex(void* v_self, float s, float w) {
	ParaTree* self = (ParaTree*)v_self;
	MeshInstancePtr leaf = self->leafMesh->newInstance(Materials::bark);
	leaf->translate(glm::vec3(0, 0, -1)); // set origin to base of leaf
	//leaf->scale(glm::vec3(3, 3, 3));
	leaf->scale(glm::vec3(20, 20, 20)); // set base leaf size
	leaf->rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));
	leaf->applyMatrix(self->turtle.getMatrixWidthOnly());
}

void ParaTree::setPosition(glm::vec3 position) {
	this->position = position;
}

void ParaTree::createBoundingVolume(){
	GLfloat BoundingX = this->getPosition().x;
	GLfloat BoundingY = this->getPosition().y + 2.0;
	GLfloat BoundingZ = this->getPosition().z;

	 BS_center = glm::vec3(BoundingX, BoundingY, BoundingZ);
	
}

glm::vec3 ParaTree::getCenterCoord(){
	this->createBoundingVolume();
	return BS_center;
}



// tree parameter presets

const ParaTree::TreeParams ParaTree::Presets::a = {0.75, 0.77, 35, -35,   0,   0, 30, 0.50, 0.40,  0.0, 10};
const ParaTree::TreeParams ParaTree::Presets::b = {0.65, 0.71, 27, -68,   0,   0, 20, 0.53, 0.50,  1.7, 12};
const ParaTree::TreeParams ParaTree::Presets::c = {0.50, 0.85, 25, -15, 180,   0, 20, 0.45, 0.50,  0.5,  9};
const ParaTree::TreeParams ParaTree::Presets::d = {0.60, 0.85, 25, -15, 180, 180, 20, 0.45, 0.50,  0.0, 10};
const ParaTree::TreeParams ParaTree::Presets::e = {0.58, 0.83, 30,  15,   0, 180, 20, 0.40, 0.50,  1.0, 11};
const ParaTree::TreeParams ParaTree::Presets::f = {0.92, 0.37,  0,  60, 180,   0,  8, 0.50, 0.00,  0.5, 15};
const ParaTree::TreeParams ParaTree::Presets::g = {0.80, 0.80, 30, -30, 137, 137, 30, 0.50, 0.50,  0.0, 10};
const ParaTree::TreeParams ParaTree::Presets::h = {0.95, 0.75,  5, -30, -90,  90, 40, 0.60, 0.45, 25.0, 12};
const ParaTree::TreeParams ParaTree::Presets::i = {0.55, 0.95, -5,  30, 137, 137,  8, 0.40, 0.00,  5.0, 12};

const ParaTree::TreeParams ParaTree::Presets::d2 = {0.75,  0.9, 30, -25, 160, 160, 60, 0.45, 0.50, 0.0, 10};
const ParaTree::TreeParams ParaTree::Presets::e2 = {0.58, 0.83, 30,  15, 172, 160, 40, 0.40, 0.50, 1.0, 11};
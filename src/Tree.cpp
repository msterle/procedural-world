#include "Tree.h"

#include <vector>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

#include "../include/glm/gtc/type_ptr.hpp"

#include "Vertex.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
#include "LSystem.h"
#include "Turtle.h"
#include "helpers.h"

using namespace std;


// Constructor

Tree::Tree(float n) : angle(20) {
	lsys = LSystem<callback_t>(
		"X",
		LSystem<callback_t>::Grammar {
			// variables
			{
				'X',
				LSystem<callback_t>::Rule {
					.replace = "F-[[X]+X]+F[+FX]-X",
					.action = Tree::lsysData::noop,
					.actionArg = (void*)this
				}
			},
			{
				'F',
				LSystem<callback_t>::Rule {
					.replace = "FF",
					.action = Tree::lsysData::forward,
					.actionArg = (void*)this
				}
			},
			// constants
			{
				'+',
				LSystem<callback_t>::Rule {
					.replace = "+",
					.action = Tree::lsysData::right,
					.actionArg = (void*)this
				}
			},
			{
				'-',
				LSystem<callback_t>::Rule {
					.replace = "-",
					.action = Tree::lsysData::left,
					.actionArg = (void*)this
				}
			},
			{
				'[',
				LSystem<callback_t>::Rule {
					.replace = "[",
					.action = Tree::lsysData::push,
					.actionArg = (void*)this
				}
			},
			{
				']',
				LSystem<callback_t>::Rule {
					.replace = "]",
					.action = Tree::lsysData::pop,
					.actionArg = (void*)this
				}
			}
		});

	string objPath = PathHelper::model("cylinder8.obj");
	cylinderMesh = newMesh(objPath);

	lsys.iterate(n);
	lsys.run();
	cout << "instances: " << meshes.front().getNumInstances() << endl;
}

// protected static functions

void Tree::lsysData::forward(void* v_self) {
	Tree* self = (Tree*)v_self;
	// advance turtle
	self->turtle.forward(1);
	// place new segment behind
	MeshInstancePtr segment = self->cylinderMesh->newInstance(Materials::pewter);
	segment->scale(glm::vec3(1, 1, 1)); // set height
	segment->rotate(glm::radians(90.0f), glm::vec3(1, 0, 0)); // rotate to facing +Z to match turtle start orientation
	cout << "Tree: " << glm::to_string(self->turtle.getMatrix()) << endl;
	segment->applyMatrix(self->turtle.getMatrix()); // transform according to turtle
	self->turtle.scale(glm::vec3(0.95, 0.95, 0.99)); // shrink turtle
}

void Tree::lsysData::left(void* v_self) {
	Tree* self = (Tree*)v_self;
	// rotate turtle
	//self->turtle.rotate(glm::pi<float>() / 180 * self->angle, glm::vec3(1, 0, 0));
	self->turtle.turn(glm::radians(self->angle));
}

void Tree::lsysData::right(void* v_self) {
	Tree* self = (Tree*)v_self;
	// rotate turtle
	//self->turtle.rotate(glm::pi<float>() / 180 * self->angle, glm::vec3(1, 0, 0));
	self->turtle.turn(-glm::radians(self->angle));
}

void Tree::lsysData::push(void* v_self) {
	Tree* self = (Tree*)v_self;
	self->turtle.push();
}

void Tree::lsysData::pop(void* v_self) {
	Tree* self = (Tree*)v_self;
	self->turtle.pop();
}
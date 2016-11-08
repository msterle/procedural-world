#ifndef TREE_H
#define TREE_H

#include <vector>
#include <string>

#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "LSystem.h"
#include "Turtle.h"

using namespace std;


class Tree : public Model {
protected:
	typedef void (callback_t)(void*);
	LSystem<callback_t> lsys;
	struct lsysData {
		static void forward(void* v_self);
		static void left(void* v_self);
		static void right(void* v_self);
		static void push(void* v_self);
		static void pop(void* v_self);
		static void noop(void* v_self) {}
	};
	Turtle turtle;
	Mesh* cylinderMesh;
	float angle;
public:
	Tree(float angle = 20);
};

#endif
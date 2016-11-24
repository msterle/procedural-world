#ifndef PARATREE_H
#define PARATREE_H

#include <vector>
#include <string>

#include "Shader.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "PLS.h"
#include "Turtle.h"


using namespace std;


class ParaTree : public Model {
public:
	//Tree box positions
	float top, bot, right, left, front, back;
	struct TreeParams {
		float r1, r2, a1, a2, f1, f2, w0, q, e, min, n;
	};
	struct Presets {
		static const TreeParams a, b, c, d, e, f, g, h, i, d2;
	};
protected:
	PLS treePLS;
	Turtle turtle;
	Mesh* cylinderMesh;
	glm::vec3 position, BS_center, BS_radius;
	void generate(unsigned int n);
	struct Actions {
		static void setWidth(void* v_self, float w) {
			((ParaTree*)v_self)->turtle.setWidth(w / 150);
		}
		static void forward(void* v_self, float length = 1);
		static void turnLeft(void* v_self, float angle = 20) {
			((ParaTree*)v_self)->turtle.turn(glm::radians(-angle));
		}
		static void turnRight(void* v_self, float angle) {
			((ParaTree*)v_self)->turtle.turn(glm::radians(angle));
		}
		static void rollLeft(void* v_self, float angle) {
			((ParaTree*)v_self)->turtle.roll(glm::radians(angle));
		}
		static void rollRight(void* v_self, float angle) {
			((ParaTree*)v_self)->turtle.roll(glm::radians(-angle));
		}
		static void push(void* v_self) { ((ParaTree*)v_self)->turtle.push(); }
		static void pop(void* v_self) { ((ParaTree*)v_self)->turtle.pop(); }
		static void noop() {}
	};
public:
	ParaTree(TreeParams treeParams = Presets::h);
	glm::vec3 getPosition();
	Mesh* getCylinderMesh(){ return cylinderMesh; }
	void setPosition(glm::vec3 position);
	//Create bounding sphere
	void createBoundingVolume();
	glm::vec3 getCenterCoord();

};

#endif
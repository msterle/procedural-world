#ifndef TURTLE_H
#define TURTLE_H

#include <stack>

#include "../include/glm/gtc/type_ptr.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/gtx/matrix_decompose.hpp"
#include "../include/glm/gtc/quaternion.hpp"

// debug only
#include "../include/glm/gtx/string_cast.hpp"
using namespace std;

class Turtle {
protected:
	struct TransformationSet {
		glm::vec3 scaleVec;
		glm::quat orientQuat;
		glm::vec3 translVec;
	} transComponents;
	stack<TransformationSet> transStack;
public:
	Turtle() {
		// initialize turtle facing up
		transComponents  = { 
			glm::vec3(1), 
			glm::rotate(glm::quat(), glm::radians(-90.0f), glm::vec3(1, 0, 0)), 
			glm::vec3(0) 
		};
	} 
	void scale(glm::vec3 s) {
		transComponents.scaleVec *= s;
	}
	void rotate(float angle, glm::vec3 axis) {
		transComponents.orientQuat = glm::rotate(transComponents.orientQuat, angle, axis);
	}
	void turn(float angle) {
		transComponents.orientQuat = glm::rotate(transComponents.orientQuat, 
			angle, glm::vec3(0, 1, 0));
	}
	void pitch(float angle) {
		transComponents.orientQuat = glm::rotate(transComponents.orientQuat, 
			angle, glm::vec3(1, 0, 0));
	}
	void roll(float angle) {
		transComponents.orientQuat = glm::rotate(transComponents.orientQuat, 
			angle, glm::vec3(0, 0, 1));
	}
	void forward(float t) {
		// apply scale and rotation to up vector of specified length
		transComponents.translVec += glm::vec3(glm::mat4_cast(transComponents.orientQuat) 
			* glm::vec4(0, 0, t * transComponents.scaleVec.z, 0));
	}
	void setLength(float l) {
		transComponents.scaleVec.z = l;
	}
	void setWidth(float w) {
		transComponents.scaleVec.x = w;
		transComponents.scaleVec.y = w;
	}
	glm::mat4 getMatrix() {
		// scale, translate, then rotate
		return glm::translate(glm::mat4(1), transComponents.translVec)
			* glm::mat4_cast(transComponents.orientQuat)
			* glm::scale(glm::mat4(1), transComponents.scaleVec);
	}
	glm::mat4 getMatrixWidthOnly() {
		// scale, translate, then rotate
		return glm::translate(glm::mat4(1), transComponents.translVec)
			* glm::mat4_cast(transComponents.orientQuat)
			* glm::scale(glm::mat4(1), glm::vec3(transComponents.scaleVec.x, transComponents.scaleVec.y, 1));
	}
	/*
	glm::mat4 setMatrix(glm::mat4 transformMat) {
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transformMat, 
			transComponents.scaleVec, 
			transComponents.orientQuat, 
			transComponents.translVec, 
			skew, 
			perspective); // ignore skew and perspective
	}
	*/
	void push() {
		transStack.push(transComponents);
	}
	void pop() {
		transComponents = transStack.top();
		transStack.pop();
	}
};

#endif
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
	Turtle() : transComponents { glm::vec3(1), glm::quat(), glm::vec3(0) } { }
	void scale(glm::vec3 s) {
		transComponents.scaleVec *= s;
	}
	void rotate(float angle, glm::vec3 axis) {
		transComponents.orientQuat = glm::rotate(transComponents.orientQuat, angle, axis);
	}
	void forward(float t) {
		// apply scale and rotation to up vector of specified length
		transComponents.translVec += glm::vec3(glm::mat4_cast(transComponents.orientQuat) 
			* glm::vec4(0, t * transComponents.scaleVec.y, 0, 0));
	}
	glm::mat4 getMatrix() {
		// scale, translate, then rotate
		return glm::translate(glm::mat4(1), transComponents.translVec)
			* glm::mat4_cast(transComponents.orientQuat)
			* glm::scale(glm::mat4(1), transComponents.scaleVec);
	}
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
	void push() {
		transStack.push(transComponents);
	}
	void pop() {
		transComponents = transStack.top();
		transStack.pop();
	}
};

#endif
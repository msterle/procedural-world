#ifndef MODELNEW_H
#define MODELNEW_H

#include "../include/glm/gtc/type_ptr.hpp"

using namespace std;

class ModelNew {
public:
	ModelNew();
	glm::mat4 modelMat = glm::mat4(1);
	unsigned int getIndex() const { return this->modelIndex; }
	void setIndex(unsigned int modelIndex) { this->modelIndex = modelIndex; }
	void translate(glm::vec3 t);
	void scale(glm::vec3 s);
	void rotate(float angle, glm::vec3 axis);
	~ModelNew();
protected:
	unsigned int modelIndex;
};

#endif
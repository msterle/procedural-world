#ifndef CAMERA_H
#define CAMERA_H

#include "../include/glm/glm.hpp"

static const float maxAbsoluteAttitude = 89.0f;

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 orientation);
	Camera() : Camera(glm::vec3(0, 0, 1), glm::vec3(-glm::half_pi<float>(), 0, 0)) {};
	glm::vec3 getPosition();
	void setPosition(glm::vec3 position);
	void translate(glm::vec3 t);
	void moveRelative(glm::vec3 m);
	glm::vec3 getOrientation();
	glm::mat4 getOrientationMat();
	void setOrientation(glm::vec3 orientation);
	void setHeading(float angle);
	void setAttitude(float angle);
	void setBank(float angle);
	void yaw(float angle);
	void pitch(float angle);
	void roll(float angle);
	void lookAt(glm::vec3 target);
	glm::mat4 getViewMat();
protected:
	glm::vec3 position, orientation; // orientation in degrees (yaw, pitch, roll)
	glm::vec3 getForward();
	glm::vec3 getUp();
	glm::vec3 getRight();
};

#endif
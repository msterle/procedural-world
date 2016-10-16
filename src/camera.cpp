#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"
#include <cmath>

using namespace std;

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


// Public methods

Camera::Camera(glm::vec3 position, glm::vec3 orientation) {
	this->setPosition(position);
	this->setOrientation(orientation);
}

glm::vec3 Camera::getPosition() {
	return this->position;
}

void Camera::setPosition(glm::vec3 position) {
	this->position = position;
}

void Camera::translate(glm::vec3 t) {
	this->position += t;
}

void Camera::moveRelative(glm::vec3 m) {
	this->translate(m[0] * this->getRight()
		+ m[1] * this->getUp()
		+ m[2] * this->getForward());
}

glm::vec3 Camera::getOrientation() {
	return this->orientation;
}

glm::mat4 Camera::getOrientationMat() {
	glm::mat4 orientationMat;
	orientationMat = glm::rotate(orientationMat, glm::radians(-this->orientation[1]), glm::vec3(1, 0, 0));
	orientationMat = glm::rotate(orientationMat, glm::radians(this->orientation[0]), glm::vec3(0, 1, 0));
	return orientationMat;
}

void Camera::setOrientation(glm::vec3 orientation) {
	this->setHeading(orientation[0]);
	this->setAttitude(orientation[1]);
	this->setBank(orientation[2]);
}

void Camera::setHeading(float angle) {
	// wrap heading between -180 and 180
	this->orientation[0] = angle - 360.0f * floor((angle + 180.0f) / 360.0f);
}

void Camera::setAttitude(float angle) {
	// keep attitude within bounds
	if(angle < -maxAbsoluteAttitude)
		angle = -maxAbsoluteAttitude;
	if(angle > maxAbsoluteAttitude)
		angle = maxAbsoluteAttitude;
	this->orientation[1] = angle; 
}

void Camera::setBank(float angle) {
	// wrap bank between -180 and 180
	this->orientation[2] = angle - 360.0f * floor((angle + 180.0f) / 360.0f);
}

void Camera::yaw(float angle) {
	this->setHeading(this->orientation[0] + angle);
}

void Camera::pitch(float angle) {
	this->setAttitude(this->orientation[1] + angle);
}

void Camera::roll(float angle) {
	this->setBank(this->orientation[2] + angle);
}

// METHOD IS CORRECT
void Camera::lookAt(glm::vec3 target) {
	if(target == this->position)
		return;
	glm::vec3 direction = glm::normalize(target - this->position);
	this->setHeading(-glm::degrees(glm::atan(-direction.x, -direction.z)));
	this->setAttitude(-glm::degrees(glm::asin(-direction.y)));
	this->setBank(0.0f);
}

glm::mat4 Camera::Camera::getViewMat() {
	return this->getOrientationMat() * glm::translate(glm::mat4(), -this->position);
}


// Protected methods
glm::vec3 Camera::getForward() {
	return glm::vec3(glm::inverse(this->getOrientationMat()) * glm::vec4(0, 0, -1, 1));
}

glm::vec3 Camera::getUp() {
	return glm::vec3(glm::inverse(this->getOrientationMat()) * glm::vec4(0, 1, 0, 1));
}

glm::vec3 Camera::getRight() {
	return glm::vec3(glm::inverse(this->getOrientationMat()) * glm::vec4(1, 0, 0, 1));
}
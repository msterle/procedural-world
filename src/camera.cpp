#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/constants.hpp"
#include "../include/glm/gtx/euler_angles.hpp"

#include <cmath>

using namespace std;

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 orientation);
	Camera() : Camera(glm::vec3(0, 0, 1), glm::vec3(-glm::half_pi<float>(), 0, 0)) {};
	glm::vec3 getPosition();
	glm::vec3 getOrientation();
	glm::vec3 getDirection();
	void setPosition(glm::vec3 position);
	void translate(glm::vec3 t);
	void moveRelative(glm::vec3 m);
	void setOrientation(glm::vec3 orientation);
	void setHeading(float angle);
	void setAttitude(float angle);
	void setBank(float angle);
	void yaw(float angle);
	void pitch(float angle);
	void roll(float angle);
	void lookAt(glm::vec3 target);
	glm::mat4 getViewMat();

private:
	glm::vec3 position, orientation; // orientation form (yaw, pitch, roll)
};

Camera::Camera(glm::vec3 position, glm::vec3 orientation) {
	this->setPosition(position);
	this->setOrientation(orientation);
}

glm::vec3 Camera::getPosition() {
	return this->position;
}

glm::vec3 Camera::getOrientation() {
	return this->orientation;
}

glm::vec3 Camera::getDirection() {
	return glm::normalize(-this->getViewMat()[1]);
}

void Camera::setPosition(glm::vec3 position) {
	this->position = position;
}

void Camera::translate(glm::vec3 t) {
	this->position += t;
}

// Move relative to camera orientation (+X = right, +Y = up, +Z = forward)
void Camera::moveRelative(glm::vec3 m) {
	glm::mat4 mat = this->getViewMat();
	this->translate(m[0] * glm::normalize(mat[0])
		+ m[1] * glm::normalize(mat[1])
		+ m[2] * glm::normalize(mat[2]));
}

void Camera::setOrientation(glm::vec3 orientation) {
	this->setHeading(orientation[0]);
	this->setAttitude(orientation[1]);
	this->setBank(orientation[2]);
}

void Camera::setHeading(float angle) {
	// keep between -pi and pi
	this->orientation[0] = fmod(angle + glm::pi<float>(), 2 * glm::pi<float>()) - glm::pi<float>();
}

void Camera::setAttitude(float angle) {
	// keep between -pi/2 and pi/2
	if(angle < -glm::pi<float>())
		angle = -glm::pi<float>();
	if(angle > glm::pi<float>())
		angle = glm::pi<float>();
	this->orientation[1] = angle; 
}

void Camera::setBank(float angle) {
	// keep between -pi and pi
	this->orientation[2] = fmod(angle + glm::pi<float>(), 2 * glm::pi<float>()) - glm::pi<float>();
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

void Camera::lookAt(glm::vec3 target) {
	glm::vec3 direction = glm::normalize(target - this->position);
	this->setHeading(glm::atan(direction.x, direction.z));
	this->setAttitude(glm::asin(direction.y));
	this->setBank(0.0f);
}

glm::mat4 Camera::Camera::getViewMat() {
	return glm::translate(glm::yawPitchRoll(orientation[0], orientation[1], orientation[2]), position);
}
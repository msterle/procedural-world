#include "../include/glew.h"
#include "../include/glfw3.h"

#include "UI.h"

#include "World.h"
#include "Camera.h"

using namespace std;

namespace UI {
	const float MOUSE_SENS_MOV = 0.05f;
	const float CAMERA_FOV = 45.0f; // camera field of view
	World* world;
	GLFWwindow* window;

	UIBase* UIBase::activeUI;
	UIExplore Explore;

	// Public methods

	// Initialize UI elements
	void init(World* world, GLFWwindow* window) {
		UI::world = world;
		UI::window = window;

		// Set to catch state change between polling cycles
		glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

		// Set window callback functions
		glfwSetKeyCallback(window, UI::UIBase::onKeyDispatcher);
		glfwSetMouseButtonCallback(window, UI::UIBase::onMouseButtonDispatcher);
		glfwSetCursorPosCallback(window, UI::UIBase::onCursorMoveDispatcher);
		glfwSetFramebufferSizeCallback(window, UI::UIBase::onResizeDispatcher);

		// Initialize camera for window
		int width, height;
		glfwGetFramebufferSize(UI::window, &width, &height);
		UI::world->camera.perspective(glm::radians(UI::CAMERA_FOV),
			(float)width / (float)height,
			0.01f,
			15000.0f
			);
	}

	void setActive(UIBase &UIinstance) {
		UIinstance.setActive();
	}

	// IUExplore event callbacks

	void UIExplore::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if(action == GLFW_PRESS || action == GLFW_REPEAT) {
			switch(key) {
			// Rotate model around world axes
			case GLFW_KEY_X:
				UI::world->terrain.rotate(glm::radians(10.0f),
					glm::vec3((mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f, 0.0f ));
				break;
			case GLFW_KEY_Y:
				UI::world->terrain.rotate(glm::radians(10.0f), 
					glm::vec3(0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f));
				break;
			case GLFW_KEY_Z:
				UI::world->terrain.rotate(glm::radians(10.0f), 
					glm::vec3(0.0f, 0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f)));
				break;
			case GLFW_KEY_LEFT:
				UI::world->camera.moveRelative(glm::vec3(-0.1, 0, 0));
				break;
			case GLFW_KEY_RIGHT:
				UI::world->camera.moveRelative(glm::vec3(0.1, 0, 0));
				break;
			case GLFW_KEY_UP:
				UI::world->camera.moveRelative(glm::vec3(0, 0.1, 0));
				break;
			case GLFW_KEY_DOWN:
				UI::world->camera.moveRelative(glm::vec3(0, -0.1, 0));
				break;
			case GLFW_KEY_T:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case GLFW_KEY_W:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GLFW_KEY_P:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			}
		}
	}

	void UIExplore::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
		if( action == GLFW_PRESS && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) ) {
			// store cursor position
			glfwGetCursorPos(window, &this->cursorLastX, &this->cursorLastY);
		}
	}

	void UIExplore::onCursorMove(GLFWwindow* window, double xpos, double ypos) {
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// change heading and attitude
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			float anglePerPixel = UI::CAMERA_FOV / height;
			UI::world->camera.yaw((this->cursorLastX - xpos) * anglePerPixel);
			UI::world->camera.pitch((ypos - this->cursorLastY) * anglePerPixel);
			this->cursorLastX = xpos;
			this->cursorLastY = ypos;
		}
		if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			// move camera in/out
			UI::world->camera.moveRelative(glm::vec3(0, 0, this->cursorLastY - ypos) * UI::MOUSE_SENS_MOV);
			// store cursor position
			this->cursorLastX = xpos;
			this->cursorLastY = ypos;
		}
	}

	void UIExplore::onResize(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		UI::world->camera.perspective(glm::radians(UI::CAMERA_FOV),
			(float)width / (float)height,
			0.01f,
			15000.0f
			);
	}

}
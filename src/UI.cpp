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
	UIWalk walkMode;

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
			1000.0f
			);
	}

	void setActive(UIBase &UIinstance) {
		UIinstance.setActive();
	}

	// UIExplore event callbacks

	void UIExplore::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		//Display cursor and unlock it from the frame
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
			case GLFW_KEY_V:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GLFW_KEY_P:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
				//Switch Control Mode
			case GLFW_KEY_H:
				UI::setActive(UI::walkMode);
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
		//Display cursor and lock it to the frame
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
			1000.0f
			);
	}

	// UIWalk event callbacks
	void UIWalk::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		GLfloat mapheight = world->terrain.getYAtXZWorld(world->camera.getPosition().x, UI::world->camera.getPosition().z) + 4.0;
		glm::vec3 newPos = (glm::vec3(UI::world->camera.getPosition().x, world->terrain.getYAtXZWorld(world->camera.getPosition().x, UI::world->camera.getPosition().z) + 4.0, 
			UI::world->camera.getPosition().z) + glm::vec3(0, 0, 0.3));
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			switch (key) {
				//Rotate Mesh Around X axis
			case GLFW_KEY_Y:
				UI::world->terrain.rotate(glm::radians(10.0f),
					glm::vec3(0.0f, (mods == GLFW_MOD_SHIFT ? 1.0f : -1.0f), 0.0f));
				break;
			case GLFW_KEY_T:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case GLFW_KEY_V:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GLFW_KEY_P:
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				break;
			case GLFW_KEY_W:
				//Lock camera position terrain height.
				UI::world->camera.setPosition(glm::vec3(world->camera.getPosition().x, 
					mapheight, world->camera.getPosition().z));
				cout << "Height Value : " << mapheight << endl;
				cout << "New Pos Height Value : " << newPos.y << endl;
				UI::world->camera.moveRelative(glm::vec3(0, 0, 0.3));
				break;
			case GLFW_KEY_A:
				//Lock camera position terrain height.
				UI::world->camera.setPosition(glm::vec3(UI::world->camera.getPosition().x, UI::world->terrain.getYAtXZWorld(UI::world->camera.getPosition().x, UI::world->camera.getPosition().z) + 4.0, UI::world->camera.getPosition().z));
				UI::world->camera.moveRelative(glm::vec3(-0.3, 0, 0));
				break;
			case GLFW_KEY_S:
				//Lock camera position terrain height.
				UI::world->camera.setPosition(glm::vec3(UI::world->camera.getPosition().x, UI::world->terrain.getYAtXZWorld(UI::world->camera.getPosition().x, UI::world->camera.getPosition().z) + 4.0, UI::world->camera.getPosition().z));
				UI::world->camera.moveRelative(glm::vec3(0, 0, -0.3));
				break;
			case GLFW_KEY_D:
				//Lock camera position terrain height.
				UI::world->camera.setPosition(glm::vec3(UI::world->camera.getPosition().x, UI::world->terrain.getYAtXZWorld(UI::world->camera.getPosition().x, UI::world->camera.getPosition().z) + 4.0, UI::world->camera.getPosition().z));
				UI::world->camera.moveRelative(glm::vec3(0.3, 0, 0));
				break;
				//Switch Control Mode
			case GLFW_KEY_H:
				UI::setActive(UI::Explore);
				break;
			}
		}
	}

	void UIWalk::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS && (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)) {
			// store cursor position
			glfwGetCursorPos(window, &this->cursorLastX, &this->cursorLastY);
		}
	}

	void UIWalk::onCursorMove(GLFWwindow* window, double xpos, double ypos) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// change heading and attitude
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			float anglePerPixel = UI::CAMERA_FOV / height;
			UI::world->camera.yaw((xpos - this->cursorLastX) * anglePerPixel);
			UI::world->camera.pitch((this->cursorLastY - ypos) * anglePerPixel);
			this->cursorLastX = xpos;
			this->cursorLastY = ypos;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			// move camera in/out
			UI::world->camera.moveRelative(glm::vec3(0, 0, this->cursorLastY - ypos) * UI::MOUSE_SENS_MOV);
			// store cursor position
			this->cursorLastX = xpos;
			this->cursorLastY = ypos;
		}
	}

	void UIWalk::onResize(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
		UI::world->camera.perspective(glm::radians(UI::CAMERA_FOV),
			(float)width / (float)height,
			0.01f,
			1000.0f
			);
	}

}
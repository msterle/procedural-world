#ifndef UI_H
#define UI_H

#include "../include/glew.h"
#include "../include/glfw3.h"

class World;
class Camera;

namespace UI {
	// Abstract base class
	class UIBase {
		// Event callbacks to be defined by children
		virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
		virtual void onCursorMove(GLFWwindow* window, double xpos, double ypos) = 0;
		virtual void onResize(GLFWwindow* window, int width, int height) = 0;
		
		static UIBase* UIinstance;
	public:
		virtual void setActive() { UIinstance = this; }
		
		static void onKeyDispatcher(GLFWwindow* window, int key, int scancode, int action, int mods) {
			if(UIinstance)
				UIinstance->onKey(window, key, scancode, action, mods);
		}
		static void onMouseButtonDispatcher(GLFWwindow* window, int button, int action, int mods) {
			if(UIinstance)
				UIinstance->onMouseButton(window, button, action, mods);
		}
		static void onCursorMoveDispatcher(GLFWwindow* window, double xpos, double ypos) {
			if(UIinstance)
				UIinstance->onCursorMove(window, xpos, ypos);
		}
		static void onResizeDispatcher(GLFWwindow* window, int width, int height) {
			if(UIinstance)
				UIinstance->onResize(window, width, height);
		}
	};

	// UI for walk mode class
	class UIWalk : public UIBase {
	public:
		UIWalk();
		virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods);
		virtual void onCursorMove(GLFWwindow* window, double xpos, double ypos);
		virtual void onResize(GLFWwindow* window, int width, int height);
	protected:
		double cursorLastX, cursorLastY;
	};
	extern UIWalk* walk;

	void init(World* world, GLFWwindow* window);
	void setActive(UIBase* UIinstance);

}

#endif
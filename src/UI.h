#ifndef UI_H
#define UI_H

#include <vector>
#include "World.h"
#include "Camera.h"

struct GLFWwindow;
class World;
class Camera;

using namespace std;

namespace UI {
	// Abstract base class
	class UIBase {
	public:
		// Set instance as active for static dispatchers
		virtual void setActive() { activeUI = this; }
		// Static dispatchers
		static void onKeyDispatcher(GLFWwindow* window, int key, int scancode, int action, int mods) {
			if(activeUI)
				activeUI->onKey(window, key, scancode, action, mods);
		}
		static void onMouseButtonDispatcher(GLFWwindow* window, int button, int action, int mods) {
			if(activeUI)
				activeUI->onMouseButton(window, button, action, mods);
		}
		static void onCursorMoveDispatcher(GLFWwindow* window, double xpos, double ypos) {
			if(activeUI)
				activeUI->onCursorMove(window, xpos, ypos);
		}
		static void onResizeDispatcher(GLFWwindow* window, int width, int height) {
			if(activeUI)
				activeUI->onResize(window, width, height);
		}
	private:
		// Event callbacks to be defined by subclasses
		virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods) = 0;
		virtual void onCursorMove(GLFWwindow* window, double xpos, double ypos) = 0;
		virtual void onResize(GLFWwindow* window, int width, int height) = 0;
		// Active subclass instance
		static UIBase* activeUI;
	};

	// UI for explore mode
	class UIExplore : public UIBase {
	private:
		virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods);
		virtual void onCursorMove(GLFWwindow* window, double xpos, double ypos);
		virtual void onResize(GLFWwindow* window, int width, int height);
		double cursorLastX, cursorLastY;
	} extern Explore;

	// UI for Walk mode
	class UIWalk : public UIBase {
	private:
		virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods);
		virtual void onCursorMove(GLFWwindow* window, double xpos, double ypos);
		virtual void onResize(GLFWwindow* window, int width, int height);
		double cursorLastX, cursorLastY;
	} extern walkMode;

	GLboolean CheckCollision(Camera* camera);
	void init(World* world, GLFWwindow* window);
	void setActive(UIBase &UIinstance);
}

#endif
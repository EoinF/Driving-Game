#include "InputManager.h";

namespace input
{
	KeyboardInput current;
	KeyboardInput previous;

	void CheckInput(GLFWwindow * window)
	{
		previous = current;
		current = KeyboardInput();
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			current.UP = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			current.LEFT = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			current.DOWN = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			current.RIGHT = true;
		}
	
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			current.W = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			current.S = true;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			current.A = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			current.D = true;
		}
		
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			current.C = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			current.Space = true;
		}
	}
}
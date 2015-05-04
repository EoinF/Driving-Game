#pragma once

#include <GLFW/glfw3.h>

namespace input
{
	struct KeyboardInput {
		bool UP, DOWN, LEFT, RIGHT,
				W, A, S, D,
				C, Space;
	};

	extern KeyboardInput current;
	extern KeyboardInput previous;

	void CheckInput(GLFWwindow * window);
}
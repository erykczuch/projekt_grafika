#include "inputHandling.h"


Handler::Handler()
{
}

void Handler::Inputs(GLFWwindow* window, bool* animation_start)
{
	// handles keys input
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && not_pressed)
	{
		std::cout << "Klawisz E" << std::endl;;
		*animation_start = true;
		not_pressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && !not_pressed)
	{
		not_pressed = true;
	}
	// handles mouse inputs
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	//{
	//	// Hides mouse cursor
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//	// Prevents camera from jumping on the first click
	//	if (firstClick)
	//	{
	//		glfwSetCursorPos(window, (width / 2), (height / 2));
	//		firstClick = false;
	//	}
	//	double mouseX;
	//	double mouseY;
	//	// Fetches the coordinates of the cursor
	//	glfwGetCursorPos(window, &mouseX, &mouseY);
	//	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	//	// and then “transforms” them into degrees
	//	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
	//	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
	//	// Calculates upcoming vertical change in the Orientation
	//	glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(- rotX), glm::normalize(glm::cross(Orientation, Up)));
	//	// Decides whether or not the next vertical Orientation is legal or not
	//		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
	//		{
	//			Orientation = newOrientation;
	//		}
	//	// Rotates the Orientation left and right
	//	Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
	//	// Sets mouse cursor to the middle of the screen so that it doesn’t end up roaming around
	//	glfwSetCursorPos(window, (width / 2), (height / 2));
	//}
	//else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	//{
	//	// Unhides cursor since camera is not looking around anymore
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//	// Makes sure the next time the camera looks around it doesn’t jump
	//	firstClick = true;
	//}
}
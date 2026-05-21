#include "inputHandling.h"


Handler::Handler()
{
}

// handles keys inputs
void Handler::Inputs(GLFWwindow* window, bool* animation_start, bool can_hit, glm::vec3* cam_pos, glm::vec3* cam_orien, float* background_rot_y, int* scene_num)
{
	// animation start
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && not_pressed_e)
	{
		if(can_hit)
		{
			*animation_start = true;
			not_pressed_e = false;
		}
	}

	// prevents the key from being pressed twice
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && !not_pressed_e)
	{
		not_pressed_e = true;
	}

	// camera position change
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && not_pressed_c)
	{
		if (*scene_num == 0)
		{
			*cam_pos = glm::vec3(-2.5f, 3.0f, 10.0f);
			*cam_orien = glm::vec3(0.0, -0.1, -1.0);
			*background_rot_y = 0.0f;
			*scene_num = 1;
		}
		else
		{
			*cam_pos = glm::vec3(-17.4337, 14.9026, 21.7259);
			*cam_orien = glm::vec3(0.33191, -0.216439, -0.91814);
			*background_rot_y = -20.0f;
			*scene_num = 0;
		}
		not_pressed_c = false;
	}

	// prevents the key from being pressed twice
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && !not_pressed_c)
	{
		not_pressed_c = true;
	}

}
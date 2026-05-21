#ifndef INPUT_CLASS_H
#define INPUT_CLASS_H
#include<GLFW/glfw3.h>
#include<glm/glm/glm.hpp>

class Handler
{
public:

	Handler();
	void Inputs(GLFWwindow* window, bool*, bool, glm::vec3*, glm::vec3*, float*, int*);

private:
	bool not_pressed_e = true;
	bool not_pressed_c = true;
};
#endif
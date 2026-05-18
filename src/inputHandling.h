#ifndef INPUT_CLASS_H
#define INPUT_CLASS_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm/glm.hpp>
#include<glm/glm/gtc/matrix_transform.hpp>
#include<glm/glm/gtc/type_ptr.hpp>
#include<glm/glm/gtx/rotate_vector.hpp>
#include<glm/glm/gtx/vector_angle.hpp>
#include"shaderClass.h"

class Handler
{
public:

	Handler();

	bool firstClick = true;
	void Inputs(GLFWwindow* window, bool*);

private:
	bool not_pressed = true;
};
#endif
#ifndef Cube_CLASS_H
#define Cube_CLASS_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "Texture.h"

class Cube
{
public:

	glm::vec3 position;
	glm::vec3 rotation; // in degrees (x, y, z)
	glm::vec3 scale;

	VAO* vao;
	Texture* tex1;
	Texture* tex2;

	Cube(glm::vec3 pos, VAO* vaoPtr, Texture* t1);

	void Draw(Shader& shader);
};

#endif
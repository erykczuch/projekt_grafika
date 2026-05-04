#include "cube.h"

Cube::Cube(glm::vec3 pos, VAO* vaoPtr, Texture* t1, Texture* t2)
{
	position = pos;
	rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);

	vao = vaoPtr;
	tex1 = t1;
	tex2 = t2;
}

void Cube::Draw(Shader& shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, scale);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

	vao->Bind();

	// for the full cube, one texture on a cube
	if (tex1 != nullptr)
	{
		tex1->texUnit(shader, "tex0", 0);
		tex1->Bind();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	// for the second half of the cube, two textures on one cube
	/*
	if (tex1 != nullptr)
	{
		tex1->texUnit(shader, "tex0", 0);
		tex1->Bind();
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}

	if (tex2 != nullptr)
	{
		tex2->texUnit(shader, "tex0", 1);
		tex2->Bind();
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)(18 * sizeof(GLuint)));
	}*/
}
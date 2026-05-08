#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "cube.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

int width = 1200;
int height = 800;

GLfloat vertices[] =
{
    // przod
    -0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,0.0f,1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,0.0f,1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,0.0f,1.0f,
    -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,0.0f,1.0f,

    // tyl
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,0.0f,-1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,0.0f,-1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,0.0f,-1.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,0.0f,-1.0f,

     // lewo
     -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,0.0f,0.0f,
     -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, -1.0f,0.0f,0.0f,
     -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, -1.0f,0.0f,0.0f,
     -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, -1.0f,0.0f,0.0f,

     // prawo
      0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,0.0f,0.0f,
      0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 1.0f,0.0f,0.0f,
      0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,0.0f,0.0f,
      0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f,0.0f,0.0f,

      // gora
      -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,1.0f,0.0f,
       0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,1.0f,0.0f,
       0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,1.0f,0.0f,
      -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,1.0f,0.0f,

      // dol
      -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,-1.0f,0.0f,
       0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,-1.0f,0.0f,
       0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f,-1.0f,0.0f,
      -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,-1.0f,0.0f
};

GLuint indices[] =
{
    0, 1, 2,  0, 2, 3,        // front
    4, 5, 6,  4, 6, 7,        // back
    8, 9,10,  8,10,11,        // left
    12,13,14, 12,14,15,       // right
    16,17,18, 16,18,19,       // top
    20,21,22, 20,22,23        // bottom
};

GLfloat lightVertices[] =
{    // COORDINATES //
    -0.1f, -0.1f, 0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, 0.1f,
    -0.1f, 0.1f, 0.1f,
    -0.1f, 0.1f, -0.1f,
     0.1f, 0.1f, -0.1f,
     0.1f, 0.1f, 0.1f
};

GLuint lightIndices[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Cubes", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);

    Shader shaderProgram("default.vert", "default.frag");
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));

    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);                    // position
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));  // color
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));  // uv
    VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));  // normal

    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    Shader lightShader("light.vert", "light.frag");
    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(0.5f, 0.5f, 0.8f, 1.0f);       // light color 
    glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);

    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 cubeModel = glm::mat4(1.0f);

    cubeModel = glm::translate(cubeModel, cubePos);

    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1,
        GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x,
        lightColor.y, lightColor.z, lightColor.w);

    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1,
        GL_FALSE, glm::value_ptr(cubeModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
        lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x,
        lightPos.y, lightPos.z);

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
        camera.Position.x, camera.Position.y, camera.Position.z);

    Texture cat("../assets/funnycat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    cat.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::vector<Cube> cubes;

    glm::vec3 cubePositions[] = {
        glm::vec3(-2.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  0.0f,  0.0f),
    };

    for (int i = 0; i < 5; i++)
    {
        Cube newCube(cubePositions[i], &VAO1, &cat);

        newCube.scale = glm::vec3(1.0f, 1.0 + i * 0.05, 1.0f);

        cubes.push_back(newCube);
    }

    float lightpos_speed = 0.1;
    float cube_anim_speed = 0.01;

    lightPos.x = 0.0;
    lightPos.y = 0.0;
    lightPos.z = 5.0;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        float time = (float)glfwGetTime();

        if (lightPos.x <= -4.0)
            lightpos_speed = 0.1;
        if (lightPos.x >= 4.0)
            lightpos_speed = -0.1;
        lightPos.x += lightpos_speed;

        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
            camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"),
            lightPos.x, lightPos.y, lightPos.z);

        VAO1.Bind();
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"),
            1, GL_FALSE, glm::value_ptr(model1));


        for (int i = 0; i < cubes.size(); i++)
        {
            if (cubes[i].scale.y >= 2.0f)
                cube_anim_speed = -0.01;
            if (cubes[i].scale.y <= 1.0f)
                cube_anim_speed = 0.01;

            cubes[i].scale.y += cube_anim_speed;

            cubes[i].Draw(shaderProgram);
        }

        lightShader.Activate();
        camera.Matrix(lightShader, "camMatrix");

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"),
            1, GL_FALSE, glm::value_ptr(lightModel));

        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    cat.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
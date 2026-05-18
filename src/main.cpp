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
#include "Model.h"
#include "inputHandling.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

int width = 800;
int height = 480;

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

// model / texture folder name
const std::string nail_mod = "nail";


bool animation_start = false;

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

    Model myModel(nail_mod, glm::vec3(0.0f, 0.0f, 0.0f));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Shader shaderProgram("default.vert", "default.frag");

    Shader lightShader("light.vert", "light.frag");
    VAO lightVAO;
    lightVAO.Bind();

    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);       // light color 
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
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 0);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1,
        GL_FALSE, glm::value_ptr(cubeModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
        lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x,
        lightPos.y, lightPos.z);

    Handler handler;

    Camera camera(width, height, glm::vec3(0.0f, 3.0f, 12.0f));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
        camera.Position.x, camera.Position.y, camera.Position.z);

    /*Texture cat("../assets/textures/funnycat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    cat.Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

    float lightpos_speed = 0.1;
    float cube_anim_speed = 0.01;

    lightPos.x = 0.0;
    lightPos.y = 0.0;
    lightPos.z = 0.0;

    Shader shadowMapShader("shadowMap.vert", "shadowMap.frag");

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaderProgram.Activate();
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "shadowMap"), 1);

    float nail_speed = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        handler.Inputs(window, &animation_start);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        float time = (float)glfwGetTime();

        /*if (lightPos.y <= -4.0)
            lightpos_speed = 0.05;
        if (lightPos.y >= 4.0)
            lightpos_speed = -0.05;
        lightPos.y += lightpos_speed;*/

        float radius = 15.0f;
        glm::vec3 lightPos;
        lightPos.x = 0 + sin(time) * radius;
        lightPos.y = 15 + 0.5f;
        lightPos.z = 6 + cos(time) * radius;

        glm::mat4 model = glm::mat4(1.0f);

        // 1. render depth of scene to texture (from light's perspective)
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 1.0f, far_plane = 25.0f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        shadowMapShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        
        myModel.DrawDepth(shadowMapShader);
        
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal using the generated depth/shadow map  
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
            camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"),
            lightPos.x, lightPos.y, lightPos.z);
            
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        myModel.Draw(shaderProgram, camera, depthMap);
        if (animation_start)
        {
            if (myModel.position.y >= 0)
                nail_speed = -0.05;
            else if (myModel.position.y <= -2)
                nail_speed = 0.05;
        }

		myModel.position.y += nail_speed;
			


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

    myModel.Destroy();

    shaderProgram.Delete();
    //cat.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
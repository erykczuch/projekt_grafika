#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "inputHandling.h"

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

// height and width of the window
int width = 1300;
int height = 860;


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

// model and texture folder name
const std::string nail_mod = "nail";
const std::string plank_mod = "wooden_plank";
const std::string garden_plane = "garden";
const std::string hammer_mod = "hammer";
const std::string garden_plane_front = "garden_front";

int scene_num = 0;

// scroll detector, used to zoom in and out by changing the FOV of the camera
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    cam->FOV -= (float)yoffset;
    if (scene_num == 1)
    {
        // min FOV value of 10
        if (cam->FOV < 10.0f)
            cam->FOV = 10.0f;

        // max FOV value of 45
        if (cam->FOV > 45.0f)
            cam->FOV = 45.0f;
    }
}

int main()
{
	// initializing GLFW and creating window
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


    // creating objects (position, scale)
    Model nailModel[2] = { Model(nail_mod, glm::vec3(-2.5f, 1.0f, 0.0f)), Model(nail_mod, glm::vec3(30.0f, 1.0f, 0.0f)) };  // two nails
    Model plankModel(plank_mod, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.1f));                                 // wooden plank
    Model gardenPlane(garden_plane, glm::vec3(20.0f, -18.0f, -80.0f), glm::vec3(2.2f, 2.2f, 1.0f));         			    // garden background
    Model gardenPlaneFront(garden_plane_front, glm::vec3(24.0f, -18.0f, -50.0f), glm::vec3(10.2f, 1.0f, 9.2f));         	// garden background front part
    Model hammerModel(hammer_mod, glm::vec3(-2.5f, 4.35f, -15.0f));         			                                    // hammmer

    // applying different transformations to the objects (rotation and blur)
    gardenPlane.rotation.y = -20.0f;
    gardenPlane.SetTextureFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    gardenPlane.SetBlurStrength(1.5f);

    gardenPlaneFront.rotation.x = 90.0f;
    gardenPlaneFront.rotation.z = 20.0f;

	// enabling depth test and backface culling, only the outwards facing triangles will be drawn
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

	// creating shaders and sending data to the shaders
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

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    lightShader.Activate();
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x,
        lightColor.y, lightColor.z, lightColor.w);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"),
        lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    // creates an object that handles the keyboard interactions
    Handler handler;

	// creating a camera object, setting its position and orientation
    Camera camera(width, height, glm::vec3(0.0f, 3.0f, 12.0f));
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
        camera.Position.x, camera.Position.y, camera.Position.z);

    camera.Position = glm::vec3(-17.4337, 14.9026, 21.7259);
    camera.Orientation = glm::vec3(0.33191, -0.216439, -0.91814);

    glfwSetWindowUserPointer(window, &camera);
    glfwSetScrollCallback(window, scroll_callback);

	// creating shaders and framebuffer for shadow mapping
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

	// variables used for the animations
    bool animation_start = false;

    float light_speed = 0.01;

    int nail_num1 = 0;
    int nail_num2 = 1;
    float nail_speed = 0.0;
    bool stop_nail_anim[3] = { true, true, true };
    bool nail_anim_start = false;

    float plank_tex_offset = 0.0;
    float plank_speed = 0.0;
    bool plank_moving = false;

    float hammer_speed = 0.0;
    bool hammer_anim_stop = false;
    int hit_num = 0;
    bool can_hit = true;
    
    // variables setting the shadow range and viewing boundaries
    float shadow_range = 25.0f;
    float near_plane = 1.0f;
    float far_plane = 100.0f;

    while (!glfwWindowShouldClose(window))
    {
		// setting a background color and clearing the back and depth buffer
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// handling the inputs
        handler.Inputs(window, &animation_start, can_hit, &camera.Position, &camera.Orientation, &gardenPlane.rotation.y, &scene_num);
        gardenPlaneFront.rotation.z = -gardenPlane.rotation.y;


        // setting the FOV to 45 whenever its the first scene (no ability to zoom there)
        if (scene_num == 0)
            camera.FOV = 45.0f;

        // updating the camera matrix and sending it to the shader
        //camera.Inputs(window);        // no camera movement
        camera.updateMatrix(0.1f, 200.0f);

        // light object animation, circular movement
        float time = (float)glfwGetTime();

        float radius = 10.0f;
        glm::vec3 lightPos;
        lightPos.x = -4.5 + sin(-time * light_speed) * radius;
        lightPos.y = 24 + 0.5f;
        lightPos.z = 0 + cos(-time * light_speed) * radius;

        // rendering depth of scene to texture (from lights perspective)
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;

        lightProjection = glm::ortho(-shadow_range, shadow_range, -shadow_range, shadow_range, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        shadowMapShader.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

		// drawing the objects to the depth map
        nailModel[0].DrawDepth(shadowMapShader);
        nailModel[1].DrawDepth(shadowMapShader);
        plankModel.DrawDepth(shadowMapShader);
        gardenPlane.DrawDepth(shadowMapShader);
        gardenPlaneFront.DrawDepth(shadowMapShader);
        hammerModel.DrawDepth(shadowMapShader);

        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // rendering scene as normal using the generated depth/shadow map
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

		// sending the camera and light position to the shader
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"),
            camera.Position.x, camera.Position.y, camera.Position.z);

        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"),
            lightPos.x, lightPos.y, lightPos.z);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glUniform2f(glGetUniformLocation(shaderProgram.ID, "texOffset"), 0.0f, 0.0f);


        // nails section
        nailModel[0].Draw(shaderProgram, camera, depthMap);
        nailModel[1].Draw(shaderProgram, camera, depthMap);


        // animation of nails getting hit
        if (nail_anim_start)
            nail_speed = -0.5;
        if (nailModel[nail_num1].position.y <= 0 && stop_nail_anim[0])
        {
            nail_speed = 0.0;
            nail_anim_start = false;
            stop_nail_anim[0] = false;
        }
        if (nailModel[nail_num1].position.y <= -1.5 && stop_nail_anim[1])
        {
            nail_speed = 0.0;
            nail_anim_start = false;
            stop_nail_anim[1] = false;
        }
        if (nailModel[nail_num1].position.y <= -3.4 && stop_nail_anim[2])
        {
            nail_speed = 0.0;
            nail_anim_start = false;
            stop_nail_anim[2] = false;
        }

        if (!stop_nail_anim[0] && !stop_nail_anim[1] && !stop_nail_anim[2])
        {
            stop_nail_anim[0] = true;
            stop_nail_anim[1] = true;
            stop_nail_anim[2] = true;
        }

        nailModel[nail_num1].position.y += nail_speed;

        float nail_last_pos = nailModel[nail_num2].position.x;

        // nail moving animation + setting the plank speed
        if (nailModel[nail_num1].position.y <= -3.4 && nailModel[nail_num2].position.x >= -2.5)
        {
            nailModel[nail_num1].position.x -= 0.18;
            nailModel[nail_num2].position.x -= 0.18;
            plank_speed = 0.00171;
            plank_moving = true;
        }

        // changing nails
        if (nailModel[nail_num2].position.x <= -2.5)
        {
            nailModel[nail_num1].position = glm::vec3(30.0f, 1.0f, 0.0f);
            int nail_change_num = nail_num1;
            nail_num1 = nail_num2;
            nail_num2 = nail_change_num;
            plank_moving = false;
        }


        // wooden plank section
		// plank texture animation
        if(fabs(nail_last_pos - nailModel[nail_num2].position.x) <= 0.0001f)
            plank_speed = 0.0;

        plank_tex_offset += plank_speed;

        glUniform2f(glGetUniformLocation(shaderProgram.ID, "texOffset"), plank_tex_offset, 0.0f);

        plankModel.Draw(shaderProgram, camera, depthMap);

        // prevents affecting other objects
        glUniform2f(glGetUniformLocation(shaderProgram.ID, "texOffset"), 0.0f, 0.0f);


        // garden section
        gardenPlane.Draw(shaderProgram, camera, depthMap);
        gardenPlaneFront.Draw(shaderProgram, camera, depthMap);


        // hammer section
        hammerModel.Draw(shaderProgram, camera, depthMap);
        
        // hitting animation
        if (animation_start && can_hit)
        {
            hammer_speed = 1.6;
            switch (hit_num)
            {
                case 0:
                {
                    if (hammerModel.rotation.x >= 72.0f)
                    {
                        nail_anim_start = true;
                    }
                    if (hammerModel.rotation.x >= 76.0f)
                    {
                        hammer_speed = -1.0;
                        hammer_anim_stop = true;
                        animation_start = false;
                        nail_anim_start = false;
                        can_hit = false;
                    }
                    break;
                }
                case 1:
                {
                    if (hammerModel.rotation.x >= 76.0f)
                        nail_anim_start = true;
                    if (hammerModel.rotation.x >= 82.2f)
                    {
                        hammer_speed = -1.0;
                        hammer_anim_stop = true;
                        animation_start = false;
                        nail_anim_start = false;
                        can_hit = false;
                    }
                    break;
                }
                case 2:
                {
                    if (hammerModel.rotation.x >= 82.2f)
                        nail_anim_start = true;
                    if (hammerModel.rotation.x >= 91.0f)
                    {
                        hammer_speed = -1.0;
                        hammer_anim_stop = true;
                        animation_start = false;
                        nail_anim_start = false;
                        can_hit = false;
                    }
                    break;
                }
            }
        }

        if (hammerModel.rotation.x <= 0.0 && hammer_anim_stop)
        {
            hammer_speed = 0.0;
            if(!plank_moving)
            {
                hit_num += 1;
                if (hit_num == 3)
                    hit_num = 0;
            }
            if (nailModel[nail_num1].position.y > -3.4 && nailModel[nail_num2].position.x > 29.9)
            {
                hammer_anim_stop = false;
                can_hit = true;
            }
        }

        hammerModel.rotation.x += hammer_speed;

		// light source rendering
        lightShader.Activate();
        camera.Matrix(lightShader, "camMatrix");

        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"),
            1, GL_FALSE, glm::value_ptr(lightModel));

        lightVAO.Bind();
		// drawing light source for debugging purposes
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	// destroying created objects and shaders
    nailModel[0].Destroy();
    nailModel[1].Destroy();
    plankModel.Destroy();
    gardenPlane.Destroy();
    gardenPlaneFront.Destroy();
    hammerModel.Destroy();

    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
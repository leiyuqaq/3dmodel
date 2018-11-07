//
//  main.cpp
//  Course 4
//
//  Created by rui huang on 10/27/17.
//  Copyright © 2017 rui huang. All rights reserved.
//


#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "SOIL2/SOIL2.h"
#include "SOIL2/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include "Light.h"
const GLint WIDTH = 800, HEIGHT = 800;
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset);
void MouseCallback( GLFWwindow *window, double xPos, double yPos);
void DoMovement();

Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat lastX = WIDTH /2.0;
GLfloat lastY = HEIGHT /2.0;
bool keys[1024];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
bool firstMouse = true;
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if (nullptr == window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Set the required callback function
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;
    
    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialise GLEW" << std::endl;
        return -1;
    }
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    Shader shader("modelLoad.vs", "modelLoad.frag");
    Shader lightShader("core.vs", "core.frag");
    // Load models
    Model ourModel("models/nanosuit.obj");
    Light lightModel = Light();
    
    glm::vec3 LightPos = glm::vec3(1.3f, 1.5f, 0.0f);
    //glm::vec3 LightPos1 = glm::vec3(0.0f, 1.5f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        
        // Get the delta time between frames
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
        LightPos = glm::rotate(LightPos, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
        /*LightPos1 = glm::rotate(LightPos1, 0.01f, glm::vec3(1.0f, 0.0f, 0.0f));*/
        glfwPollEvents();
        DoMovement();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glm::mat4 view  = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        lightShader.Use();
        model = glm::translate(model, LightPos);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lightModel.Draw(lightShader);/*
        model = glm::mat4(1.0f);
        model = glm::translate(model, LightPos1);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lightModel.Draw(lightShader);*/

        shader.Use();
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(shader.Program, "ViewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(shader.Program, "LightPos"),LightPos.x, LightPos.y, LightPos.z);
       // glUniform3f(glGetUniformLocation(shader.Program, "LightPos1"),LightPos1.x, LightPos1.y, LightPos1.z);
        glUniform1f(glGetUniformLocation(shader.Program, "material.shininess") ,64.0f );
        ourModel.Draw(shader);
        glfwSwapBuffers(window);
    }
   
    glfwTerminate();
    return 0;
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key >=0 && key < 1024)
    {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset)
{
    camera.ProcessMouseScroll(yOffset);
}
void MouseCallback( GLFWwindow *window, double xPos, double yPos)
{
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;
    
    lastX = xPos;
    lastY = yPos;
    
    // Process the camera direction
    camera.ProcessMouseMovement(xOffset, yOffset);
}
void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
}






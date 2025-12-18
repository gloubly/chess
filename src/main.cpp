#pragma once

#include "game.hpp"
#include "text_renderer.hpp"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>


Game game = Game();

int screen_width = 900;
int screen_height = 900;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
    screen_width = width;
    screen_height = height;
    
    game.setScreenDims(width, height);
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    game.handleClick(static_cast<float>(xpos), static_cast<float>(ypos), button, action, mods);
}

int main() {
	stbi_set_flip_vertically_on_load(true);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);


    game.setScreenDims(screen_width, screen_height);
    game.init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        float time = static_cast<float>(glfwGetTime());

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game.preActionCheck();
        game.render();

        glfwSwapBuffers(window);
        
        glfwWaitEvents();
        
        //glfwPollEvents();
        
        //break;
    }
    glfwTerminate();
    return 0;
}
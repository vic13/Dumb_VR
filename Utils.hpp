//
//  Utils.hpp
//  Glitter
//
//  Created by Victor Goossens on 03/11/2019.
//

#ifndef Utils_h
#define Utils_h
#include <GLFW/glfw3.h>
#include "glitter.hpp"
#include "Shader.hpp"
#include <iostream>

#endif /* Utils_h */

// Callbacks
static bool keys[1024]; // is a key pressed or not ?
                        // External static callback
                        // Is called whenever a key is pressed/released via GLFW
static bool keysPressed[1024]; // is a key pressed or not ?

static float mouseDeltaX = 0;
static float mouseDeltaY = 0;
static float prevMouseX = 0;
static float prevMouseY = 0;

float deltaTime = 0.0f;
int width;
int height;

void showFPS(void);
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/);
static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset);


GLFWwindow* init_gl() {
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "Hello World !", nullptr, nullptr);
    
    //glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Check for Valid Context
//    if (mWindow == nullptr) {
//        fprintf(stderr, "Failed to Create OpenGL Context");
//        return EXIT_FAILURE;
//    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    // Set the required callback functions
    glfwSetKeyCallback(mWindow, key_callback);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);

    // Change Viewport
    glfwGetFramebufferSize(mWindow, &width, &height);
    glViewport(0, 0, width, height);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    return mWindow;
}

void showFPS(void) {
    static double lastTime = glfwGetTime();
    static int nbFrames = 0;

    // Measure speed
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    nbFrames++;
    if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
                                         // printf and reset timer
        std::cout << 1000.0 / double(nbFrames) << " ms/frame -> " << nbFrames << " frames/sec" << std::endl;
        nbFrames = 0;
        lastTime += 1.0;
    }
}


static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/)
{
    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
        keysPressed[key] = true;
    }
        

    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, GL_TRUE);

    // V-SYNC
    if (keys[GLFW_KEY_U]) {
        static bool vsync = true;
        if (vsync) {
            glfwSwapInterval(1);
        }
        else {
            glfwSwapInterval(0);
        }
        vsync = !vsync;
    }

    if ((keys[GLFW_KEY_0] || keys[GLFW_KEY_KP_0])) {
        std::cout << "You have pressed 0" << std::endl;
    }

}

static void mouse_button_callback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        keys[GLFW_MOUSE_BUTTON_RIGHT] = true;
    else
        keys[GLFW_MOUSE_BUTTON_RIGHT] = false;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        keys[GLFW_MOUSE_BUTTON_LEFT] = true;
    else
        keys[GLFW_MOUSE_BUTTON_LEFT] = false;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        keys[GLFW_MOUSE_BUTTON_MIDDLE] = true;
    else
        keys[GLFW_MOUSE_BUTTON_MIDDLE] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouseDeltaX = xpos - prevMouseX;
    prevMouseX = xpos;
    mouseDeltaY = ypos - prevMouseY;
    prevMouseY = ypos;
    if (xpos < 0 || xpos > width/2 || ypos < -20 || ypos > height/2) {
        glfwSetCursorPos(window, width/4, height/4);
        prevMouseX = width/4;
        prevMouseY = height/4;
    }
    if (keys[GLFW_MOUSE_BUTTON_RIGHT]) {
        std::cout << "Mouse Position : (" << xpos << ", " << ypos << ")" << std::endl;
    }
}

void scroll_callback(GLFWwindow* window, double /*xoffset*/, double yoffset)
{
    if (keys[GLFW_MOUSE_BUTTON_LEFT]) {
        std::cout << "Mouse Offset : " << yoffset << std::endl;
    }
}

void clearScreen() {
    // Background Fill Color
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Clear depth buffer
    glClear(GL_DEPTH_BUFFER_BIT);
}


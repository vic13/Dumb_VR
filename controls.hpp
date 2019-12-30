#include "Constants.hpp"

// First person or third person
bool firstPerson = true;
// Flashlight on / off
bool flashlightOn = false;

// character (Steve) init position
glm::vec3 stevePos = glm::vec3(0,0,0);

// camera init rotations
float hAngle = M_PI/2;
float vAngle = 0;

// camera speed and rotation speed
float speedSlow = 0.05;
float speedBoost = 0.5;
float superSpeedBoost = 5;
float speed = speedSlow;
float rotationSpeed = 0.01;

bool pause = false;

float g = 0.3;
bool falling = false;
float fallingT = 0;

float flashlightLatency = 0.9;
glm::vec3 flashlightDirection;
glm::vec3 direction;
glm::vec3 planeDirection;
glm::vec3 right;
glm::vec3 up;


void updateFirstPerson() {
    if (keysPressed[GLFW_KEY_F5]) {
        keysPressed[GLFW_KEY_F5] = false;
        firstPerson = !firstPerson;
    }
}

void updateFlashLight() {
    flashlightDirection = glm::normalize((1.0f-flashlightLatency)*direction + flashlightLatency*flashlightDirection);
    if (keysPressed[GLFW_KEY_L]) {
        keysPressed[GLFW_KEY_L] = false;
        flashlightOn = !flashlightOn;
    }
}

bool addLight() {
    if (keysPressed[GLFW_KEY_N]) {
        keysPressed[GLFW_KEY_N] = false;
        return true;
    }
    return false;
}

void updateStevePosition() {
    planeDirection = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    
    if (keys[GLFW_KEY_W]) {
        stevePos+=speed*planeDirection;
    }
    if (keys[GLFW_KEY_S]) {
        stevePos-=speed*planeDirection;
    }
    if (keys[GLFW_KEY_D]) {
        stevePos+=speed*right;
    }
    if (keys[GLFW_KEY_A]) {
        stevePos-=speed*right;
    }
    
    if (keys[GLFW_KEY_RIGHT_SHIFT]) {
        speed = superSpeedBoost;
    }
    else if (keys[GLFW_KEY_LEFT_SHIFT]) {
        speed = speedBoost;
    } else {
        speed = speedSlow;
    }
    
    if (keysPressed[GLFW_KEY_P]) {
        keysPressed[GLFW_KEY_P] = false;
        pause = !pause;
    }
    
    if (keysPressed[GLFW_KEY_F]) {
        keysPressed[GLFW_KEY_F] = false;
        if (falling) {
            falling = false;
        } else {
            falling = true;
            fallingT = glfwGetTime();
        }
    }
    if (!falling) {
        if (keys[GLFW_KEY_SPACE]) {
            stevePos+=speed*glm::vec3(0,1,0);
        }
        if (keys[GLFW_KEY_C]) {
            stevePos-=speed*glm::vec3(0,1,0);
        }
    } else {
        float t = glfwGetTime()-fallingT;
        float v = g*t;
        stevePos -= v*glm::vec3(0,1,0);
    }
}

void updateCameraRotation() {
    vAngle -= rotationSpeed*mouseDeltaY;
    if (vAngle > M_PI/2) {
        vAngle = M_PI/2;
    }
    if (vAngle < -M_PI/2) {
        vAngle = -M_PI/2;
    }
    hAngle -= rotationSpeed*mouseDeltaX;
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    
    direction = glm::vec3(
        cos(vAngle) * sin(hAngle),
        sin(vAngle),
        cos(vAngle) * cos(hAngle)
    );
    right = glm::vec3(sin(hAngle - M_PI/2.0f), 0, cos(hAngle - M_PI/2.0f));
    up = glm::cross(right, direction);
}

#include "Constants.hpp"

// First person or third person
bool firstPerson = true;
// Flashlight on / off
bool flashlightOn = false;

// character (Steve) init position
glm::vec3 stevePos = glm::vec3(0, 18, 0);
glm::vec4 steveFront = glm::vec4(1, 0, 0, 0);
//glm::vec4 steveRight = glm::vec4(0, 0, 1, 0);


float steveLegPoints[12] = { -0.182274, 0.003461, 0.320455,   // BACK RIGHT
                             -0.182274, 0.003461, -0.32863,   // BACK LEFT
                              0.182274, 0.003461, 0.320455,   // FRONT RIGHT
                              0.182274, 0.003461, -0.328637 }; // FRONT LEFT


float steveBodyPoints[12] = {-0.302274, 0.003461, 0.650455,   // BACK RIGHT
                             -0.30274, 0.003461, -0.65863,   // BACK LEFT
                              0.302274, 0.003461, 0.650455,   // FRONT RIGHT
                              0.302274, 0.003461, -0.658637 }; // FRONT LEFT

float steveRotation = 0.0f;

int steveMoveDirections[6] = { 1, 1, 1, 1, 1, 1 };
int steveRotationDirection[2] = { 1, 1 };

glm::vec3 steveDirection;
glm::vec3 steveRight;

// camera init rotations
float hAngle = M_PI/2;
float vAngle = 0;

// camera speed and rotation speed
float speedSlow = 0.1;
float speedBoost = 0.5;
float superSpeedBoost = 5;
float speed = speedSlow;
float rotationSpeed = 0.01;

bool pause = false;
bool cameraLock = false;
bool steveRotationLock = false;

float g = 0.3;
bool fallingEnabled = false;
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
    flashlightDirection = glm::normalize((1.0f-flashlightLatency)*steveDirection + flashlightLatency*flashlightDirection);
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


glm::vec3 updateDirectionWithCollision(int moveDir, glm::vec3 direction) {
    if (!steveMoveDirections[0] && direction.x * moveDir > 0) {
        direction.x = 0.0f;
    }
    else if (!steveMoveDirections[1] && direction.x * moveDir < 0) {
        direction.x = 0.0f;
    }


    if (!steveMoveDirections[2] && direction.y * moveDir > 0) {
        direction.y = 0.0f;
    }
    else if (!steveMoveDirections[3] && direction.y * moveDir < 0) {
        direction.y = 0.0f;
    }


    if (!steveMoveDirections[4] && direction.z * moveDir > 0) {
        direction.z = 0.0f;
    }
    else if (!steveMoveDirections[5] && direction.z * moveDir < 0) {
        direction.z = 0.0f;
    }

    return direction;
}



void updateStevePosition() {
    planeDirection = glm::normalize(glm::vec3(steveDirection.x, 0, steveDirection.z));

    
    
    if (keys[GLFW_KEY_W]) {
       
        stevePos += speed * updateDirectionWithCollision(1, planeDirection);
    }
    if (keys[GLFW_KEY_S]) {
        stevePos -= speed * updateDirectionWithCollision(-1, planeDirection);
    }
    if (keys[GLFW_KEY_D]) {
        stevePos+=speed * updateDirectionWithCollision(1, steveRight);
    }
    if (keys[GLFW_KEY_A]) {
        stevePos-=speed * updateDirectionWithCollision(-1, steveRight);;
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
        if (fallingEnabled) {
            fallingEnabled = false;
        } else {
            fallingEnabled = true;
            fallingT = glfwGetTime();
        }
    }
    if (!fallingEnabled) {
        if (keys[GLFW_KEY_SPACE] && steveMoveDirections[2]) {
            stevePos+=speed*glm::vec3(0,1,0);
        }
        if (keys[GLFW_KEY_C]) {
            stevePos-=speed*glm::vec3(0,1,0);
        }
    } else {
        if (keys[GLFW_KEY_SPACE] && steveMoveDirections[2]) {
            stevePos += speed * glm::vec3(0, 1, 0);
        }
        if (steveMoveDirections[3]) {
            float t = glfwGetTime() - fallingT;
            float v = g * t;
            stevePos -= v * glm::vec3(0, 1, 0);
            //std::cout << v << std::endl;
        }
    }
}

void updateCameraLock() {
    if (keys[GLFW_KEY_T]) {
        keysPressed[GLFW_KEY_T] = false;
        cameraLock = !cameraLock;
    }

    if (keysPressed[GLFW_KEY_F4]) {
        keysPressed[GLFW_KEY_F4] = false;
        steveRotationLock = !steveRotationLock;
    }

    if (keysPressed[GLFW_KEY_F3]) {
        keysPressed[GLFW_KEY_F3] = false;
        hAngle = steveRotation + M_PI/2;
    }
}


void updateCameraRotation() {
    if (!cameraLock) {
        vAngle -= rotationSpeed * mouseDeltaY;
        if (vAngle > M_PI / 2) {
            vAngle = M_PI / 2;
        }
        if (vAngle < -M_PI / 2) {
            vAngle = -M_PI / 2;
        }
        hAngle -= rotationSpeed * mouseDeltaX;


        if (!steveRotationLock) {
            if ((rotationSpeed * mouseDeltaX > 0 && steveRotationDirection[0]) || (rotationSpeed * mouseDeltaX < 0 && steveRotationDirection[1])) {
                steveRotation -= rotationSpeed * mouseDeltaX;
            }
        }

        mouseDeltaX = 0.0;
        mouseDeltaY = 0.0;

        direction = glm::vec3(
            cos(vAngle) * sin(hAngle),
            sin(vAngle),
            cos(vAngle) * cos(hAngle)
        );


        steveDirection = glm::vec3(
            cos(vAngle) * sin(steveRotation + M_PI / 2),
            sin(vAngle),
            cos(vAngle) * cos(steveRotation + M_PI / 2)
        );

        std::cout << "steve Dir " << steveDirection.x << ", " << steveDirection.z << std::endl;
        std::cout << "steve Rot " << steveRotation << std::endl;

        right = glm::vec3(sin(hAngle - M_PI / 2.0f), 0, cos(hAngle - M_PI / 2.0f));
        steveRight = glm::vec3(sin(steveRotation), 0, cos(steveRotation));

        std::cout << "steve Right " << steveRight.x << ", " << steveRight.z << std::endl;
        up = glm::cross(right, direction);
    }
}

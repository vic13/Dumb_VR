

// camera init position
glm::vec3 camPos = glm::vec3(0,0,0);

// camera init rotations
float hAngle = 3.14/2;
float vAngle = 0;

// camera speed and rotation speed
float speedSlow = 0.01;
float speedBoost = 0.05;
float superSpeedBoost = 0.5;
float speed = speedSlow;
float rotationSpeed = 0.01;

float g = 0.3;
bool falling = false;
float fallingT = 0;

glm::vec3 direction;
glm::vec3 right;
glm::vec3 up;

void updateCameraPosition();
void updateCameraRotation();



void updateCameraPosition() {
    glm::vec3 planeDirection = glm::normalize(glm::vec3(direction.x, 0, direction.z));
    if (keys[GLFW_KEY_W]) {
        camPos+=speed*planeDirection;
    }
    if (keys[GLFW_KEY_S]) {
        camPos-=speed*planeDirection;
    }
    if (keys[GLFW_KEY_D]) {
        camPos+=speed*right;
    }
    if (keys[GLFW_KEY_A]) {
        camPos-=speed*right;
    }
    
    if (keys[GLFW_KEY_RIGHT_SHIFT]) {
        speed = superSpeedBoost;
    }
    else if (keys[GLFW_KEY_LEFT_SHIFT]) {
        speed = speedBoost;
    } else {
        speed = speedSlow;
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
            camPos+=speed*glm::vec3(0,1,0);
        }
        if (keys[GLFW_KEY_C]) {
            camPos-=speed*glm::vec3(0,1,0);
        }
    } else {
        float t = glfwGetTime()-fallingT;
        float v = g*t;
        camPos -= v*glm::vec3(0,1,0);
    }
    
    
}

void updateCameraRotation() {
    vAngle -= rotationSpeed*mouseDeltaY;
    if (vAngle > 3.14/2) {
        vAngle = 3.14/2;
    }
    if (vAngle < -3.14/2) {
        vAngle = -3.14/2;
    }
    hAngle -= rotationSpeed*mouseDeltaX;
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    
    direction = glm::vec3(
        cos(vAngle) * sin(hAngle),
        sin(vAngle),
        cos(vAngle) * cos(hAngle)
    );
    right = glm::vec3(
        sin(hAngle - 3.14f/2.0f),
        0,
        cos(hAngle - 3.14f/2.0f)
    );
    up = glm::cross(right, direction);
}

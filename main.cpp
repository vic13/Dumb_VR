// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> //print matrix/vertices

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

// Def
using namespace std;
#define VERTEX_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/triangle.vert"
#define FRAGMENT_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/triangle.frag"

//void updateCameraPosition();
GLuint createTriangleVAO();
glm::mat4 getMVP(int width, int height, float cameraX, float cameraY, float cameraZ);

// camera init position
float cameraX = 0;
float cameraY = 0;
float cameraZ = 0;


int main() {
    GLFWwindow* mWindow = init_callbacks();
    
    // Done once
    GLuint VAO = createTriangleVAO();
    Shader myShader = Shader(VERTEX_PATH, FRAGMENT_PATH, NULL, NULL, NULL);
    myShader.compile();
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        
        float timeValue = glfwGetTime();
        
        float scale = sin(timeValue);
        float intensity = (sin(timeValue) / 2.0f) + 0.5f;
        
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Update camera position
        if (keys[GLFW_KEY_W]) {
            cameraY+=0.1;
        }
        if (keys[GLFW_KEY_S]) {
            cameraY-=0.1;
        }
        if (keys[GLFW_KEY_A]) {
            cameraX-=0.1;
        }
        if (keys[GLFW_KEY_D]) {
            cameraX+=0.1;
        }
        
        // Shader
        myShader.use();
        myShader.setFloat("scale", scale);
        myShader.setFloat("intensity", intensity);
        
        // MVP
        glm::mat4 mvp = getMVP(width, height, cameraX, cameraY, cameraZ);
        myShader.setMatrix4("mvp", mvp);
        
        // Draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}

GLuint createTriangleVAO() {
    float vertices[] = {0.1f, 0.1f, 0.0f,   1.0f, 0.0f, 0.0f,
                        0.5f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
                        0.6f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,};
    
    GLuint VAO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); //color
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return VAO;
}

glm::mat4 getMVP(int width, int height, float cameraX, float cameraY, float cameraZ) {
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
      
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
    
    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
        glm::vec3(0,0,0), // and looks at the origin
        glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
    //cout << glm::to_string(View) << endl;
    glm::mat4 ViewTranslation = glm::translate(glm::mat4(1), glm::vec3(cameraX, cameraY, cameraZ));
    cout << glm::to_string(ViewTranslation) << endl;
      
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 mvp = Projection * View * ViewTranslation * Model; // Remember, matrix multiplication is the other way around
    return mvp;
}
//
//void updateCameraPosition() {
//    if (keys[GLFW_KEY_W]) {
//        cameraY+=0.1;
//    }
//    if (keys[GLFW_KEY_S]) {
//        cameraY-=0.1;
//    }
//    if (keys[GLFW_KEY_A]) {
//        cameraX-=0.1;
//    }
//    if (keys[GLFW_KEY_D]) {
//        cameraX+=0.1;
//    }
//}

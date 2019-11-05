// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"

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
using std::cout;
using std::endl;
#define VERTEX_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/triangle.vert"
#define FRAGMENT_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/triangle.frag"
#define VERTEX_PATH_TEX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/tex.vert"
#define FRAGMENT_PATH_TEX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/tex.frag"

glm::mat4 getMTriangle();
glm::mat4 getMCube();
GLuint createTriangleVAO();
GLuint createTexTriangleVAO();
GLuint createCubeVAO();
GLuint createTexture();


int main() {
    GLFWwindow* mWindow = init_gl();
    
    // Done once
    GLuint triangleVAO = createTexTriangleVAO();
    GLuint cubeVAO = createCubeVAO();
    Shader noTexShader = Shader(VERTEX_PATH, FRAGMENT_PATH, NULL, NULL, NULL);
    Shader texShader = Shader(VERTEX_PATH_TEX, FRAGMENT_PATH_TEX, NULL, NULL, NULL);
    noTexShader.compile();
    texShader.compile();
    GLuint tex1 = createTexture();
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        
        float timeValue = glfwGetTime();
        
        float scale = sin(timeValue);
        float intensity = (sin(timeValue) / 2.0f) + 0.5f;
        
        clearScreen();
        
        // Update camera
        updateCameraPosition();
        updateCameraRotation();
        
        // Triangle
        glm::mat4 mvpTriangle = getMVP(getMTriangle());
        
        texShader.use();
        
        texShader.setMatrix4("mvp", mvpTriangle);
        
        glBindTexture(GL_TEXTURE_2D, tex1);
        glActiveTexture(GL_TEXTURE0);
        texShader.setInteger("texSampler", 0);
        
        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        
        // Cube
        glm::mat4 mvpCube = getMVP(getMCube());

        noTexShader.use();
        noTexShader.setFloat("scale", scale);
        noTexShader.setFloat("intensity", intensity);
        noTexShader.setMatrix4("mvp", mvpCube);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3*2*8);
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
                        0.6f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f};
    
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

GLuint createTexTriangleVAO() {
    float vertices[] = {0.1f, 0.1f, 0.0f,   0.0f, 0.0f,
                        0.5f, 0.5f, 0.0f,   0.5f, 1.0f,
                        0.6f, 0.0f, 0.0f,   1.0f, 0.0f};
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0); //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); //texture
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return VAO;
}

GLuint createTexture() {
    int texWidth, texHeight, n;
    unsigned char* data = stbi_load("/Users/victorgoossens/Downloads/Screenshot 2019-11-04 at 01.28.31.png", &texWidth, &texHeight, &n, 0);
    
    cout << texWidth << endl;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

GLuint createCubeVAO() {
    float posData[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    
    float colorData[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };
    
    GLuint VAO;
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    GLuint posVBO, colorVBO;
    
    glGenBuffers(1, &posVBO);
    glGenBuffers(1, &colorVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); //position
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); //color
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    return VAO;
}

glm::mat4 getMCube() {
    // cube at origin
    return glm::mat4(1.0f);
}

glm::mat4 getMTriangle() {
    // triangle at x=1
    return glm::translate(glm::mat4(1), glm::vec3(1,0,0));
}




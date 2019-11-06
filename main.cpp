// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"
#include "Model.hpp"

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

glm::mat4 getMDonough();
glm::mat4 getMSkybox();

GLuint createTexture(char* path);


int main() {
    GLFWwindow* mWindow = init_gl();
    
    // Done once
    Model* mightyDonut = new Model("/Users/victorgoossens/Desktop/donut.obj", true);
    Model* frosting = new Model("/Users/victorgoossens/Documents/Blender/donut.obj", true);
    Model* skybox = new Model("/Users/victorgoossens/Desktop/skybox.obj", false);
    
    Shader texShader = Shader(VERTEX_PATH_TEX, FRAGMENT_PATH_TEX, NULL, NULL, NULL);
    texShader.compile();
//    Shader noTexShader = Shader(VERTEX_PATH, FRAGMENT_PATH, NULL, NULL, NULL);
//    noTexShader.compile();
    
    GLuint someTex = createTexture("/Users/victorgoossens/Downloads/Screenshot 2019-11-04 at 01.16.18.png");
    GLuint deliciousTex = createTexture("/Users/victorgoossens/Documents/Blender/donut_text.png");
    GLuint skyboxTex = createTexture("/Users/victorgoossens/Desktop/skybox.png");
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        
//        float timeValue = glfwGetTime();
//        float scale = sin(timeValue);
//        float intensity = (sin(timeValue) / 2.0f) + 0.5f;
        
        clearScreen();
        
        // Update camera
        updateCameraPosition();
        updateCameraRotation();
        
        
        
        texShader.use();
        
        glm::mat4 mvpDonough = getMVP(getMDonough());
        texShader.setMatrix4("mvp", mvpDonough);
        glBindTexture(GL_TEXTURE_2D, deliciousTex);
        mightyDonut->Draw(texShader);
//        glActiveTexture(GL_TEXTURE0);             What's is that for ?
//        texShader.setInteger("texSampler", 0);
        
        glBindTexture(GL_TEXTURE_2D, someTex);
        frosting->Draw(texShader);
        
        glm::mat4 mvpSkybox = getMVP(getMSkybox());
        texShader.setMatrix4("mvp", mvpSkybox);
        glBindTexture(GL_TEXTURE_2D, skyboxTex);
        skybox->Draw(texShader);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}

GLuint createTexture(char* path) {
    int texWidth, texHeight, n;
    unsigned char* data = stbi_load(path, &texWidth, &texHeight, &n, 0);
    
    cout << texWidth << endl;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

glm::mat4 getMDonough() {
    return glm::scale(glm::mat4(1), glm::vec3(5,5,5));
}

glm::mat4 getMSkybox() {
    return glm::scale(glm::mat4(1), glm::vec3(1000,1000,1000));
}



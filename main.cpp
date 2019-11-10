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
#define VERTEX_PATH_TEX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/tex.vert"
#define FRAGMENT_PATH_TEX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/tex.frag"
#define VERTEX_PATH_SKYBOX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/skybox.vert"
#define FRAGMENT_PATH_SKYBOX "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/skybox.frag"

glm::mat4 getV();
glm::mat4 getP();

int main() {
    GLFWwindow* mWindow = init_gl();
    
    // Shaders
    Shader texShader = Shader(VERTEX_PATH_TEX, FRAGMENT_PATH_TEX, NULL, NULL, NULL);
    texShader.compile();
    Shader skyboxShader = Shader(VERTEX_PATH_SKYBOX, FRAGMENT_PATH_SKYBOX, NULL, NULL, NULL);
    skyboxShader.compile();
    
    // Matrices
    glm::mat4 p = getP();
    
    // Models
    Model* donut = new Model("donut", true, 3, 0, 0, 5);
    Model* cube = new Model("cube", true, 3, 0, 1, 0.2);
    Model* skybox = new Model("skybox", false, 0, 0, 0, 1000);
    
    
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        clearScreen();
        
        // Update camera
        updateCameraPosition();
        updateCameraRotation();
        
        glm::mat4 v = getV();
        
        
        texShader.use();
        texShader.setVector3f("lightPosition", 3.0f, 3.0f, 3.0f);
        texShader.setMatrix4("v", v);
        
        // Donut
        texShader.setMatrix4("mvp", p * v * donut->m);
        texShader.setMatrix4("m", donut->m);
        glBindTexture(GL_TEXTURE_2D, donut->texture);
        donut->Draw(texShader);

        // Cube
        texShader.setMatrix4("mvp", p * v * cube->m);
        texShader.setMatrix4("m", cube->m);
        glBindTexture(GL_TEXTURE_2D, cube->texture);
        cube->Draw(texShader);
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setMatrix4("mvp", p * v * skybox->m);
        glBindTexture(GL_TEXTURE_2D, skybox->texture);
        skybox->Draw(skyboxShader);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}



glm::mat4 getP() {
    // Projection matrix : 45° Field of View, width:height ratio, display range : 0.1 unit <-> 4000 units
    return glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 4000.0f);
      
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
}

glm::mat4 getV() {
    return glm::lookAt(
    camPos,           // Position
    camPos+direction, // and looks at
    up  // Head is up (set to 0,-1,0 to look upside-down)
    );
}


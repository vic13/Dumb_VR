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
#include <list>

// Def
using std::cout;
using std::endl;
using std::list;
#define LIGHT_VERT_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/light.vert"
#define LIGHT_FRAG_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/light.frag"
#define NO_LIGHT_VERT_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/no_light.vert"
#define NO_LIGHT_FRAG_PATH "/Users/victorgoossens/Desktop/Cours/Virtual Reality/Glitter/no_light.frag"

glm::mat4 getV();
glm::mat4 getP();

int main() {
    GLFWwindow* mWindow = init_gl();
    
    // Light Pos
    float lightX = 3.0;
    float lightY = 3.0;
    float lightZ = 3.0;
    
    // Shaders
    Shader lightShader = Shader(LIGHT_VERT_PATH, LIGHT_FRAG_PATH, NULL, NULL, NULL);
    lightShader.compile();
    list<Model> lightShaderModels;
    
    Shader noLightShader = Shader(NO_LIGHT_VERT_PATH, NO_LIGHT_FRAG_PATH, NULL, NULL, NULL);
    noLightShader.compile();
    list<Model> noLightShaderModels;
    
    // Matrices
    glm::mat4 p = getP();
    
    // Models
    Model donut = Model("donut", true, 3, 0, 0, 5);
    Model cube = Model("cube", true, 3, 0, 1, 0.2);
    lightShaderModels.push_back(donut);
    lightShaderModels.push_back(cube);
    Model skybox = Model("skybox", false, 0, 0, 0, 1000);
    Model light1 = Model("light1", false, lightX, lightY, lightZ, 1);
    noLightShaderModels.push_back(skybox);
    noLightShaderModels.push_back(light1);
    
    
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        clearScreen();
        
        // Update camera
        updateCameraPosition();
        updateCameraRotation();
        
        glm::mat4 v = getV();
        
        // Models with lighting
        lightShader.use();
        lightShader.setVector3f("lightPosition", lightX, lightY, lightZ);
        lightShader.setMatrix4("v", v);
        for (Model model : lightShaderModels) {
            lightShader.setMatrix4("mvp", p * v * model.m);
            lightShader.setMatrix4("m", model.m);
            glBindTexture(GL_TEXTURE_2D, model.texture);
            model.Draw(lightShader);
        }
        
        // Models without lighting
        noLightShader.use();
        for (Model model : noLightShaderModels) {
            noLightShader.setMatrix4("mvp", p * v * model.m);
            glBindTexture(GL_TEXTURE_2D, model.texture);
            model.Draw(noLightShader);
        }

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


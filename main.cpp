// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"
#include "Model.hpp"
#include "DayNightCycle.hpp"

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
#define LIGHT_SOURCE_VERT_PATH "light_source.vert"
#define LIGHT_SOURCE_FRAG_PATH "light_source.frag"
#define LIGHT_VERT_PATH "with_light.vert"
#define LIGHT_FRAG_PATH "with_light.frag"
#define NO_LIGHT_VERT_PATH "no_light.vert"
#define NO_LIGHT_FRAG_PATH "no_light.frag"

glm::mat4 getV();
glm::mat4 getP();

int main() {
    GLFWwindow* mWindow = init_gl();
    float timeValue = glfwGetTime();
    
    //Enable blending (alpha)
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // Shaders
    Shader lightShader = Shader(LIGHT_VERT_PATH, LIGHT_FRAG_PATH, NULL, NULL, NULL);
    lightShader.compile();
    list<Model> lightShaderModels;
    
    Shader noLightShader = Shader(NO_LIGHT_VERT_PATH, NO_LIGHT_FRAG_PATH, NULL, NULL, NULL);
    noLightShader.compile();
    list<Model> noLightShaderModels;
    
    Shader lightSourceShader = Shader(LIGHT_SOURCE_VERT_PATH, LIGHT_SOURCE_FRAG_PATH, NULL, NULL, NULL);
    lightSourceShader.compile();
    
    // Matrices
    glm::mat4 p = getP();
    
    // Models
    Model donut = Model("donut", true, 3, 0, 0, 5, 2);
    Model cube = Model("cube", true, 3, 0, 1, 0.2, 5);
    Model sphere = Model("smoothSphere", true, 3, 0, -1, 4, 20);
    lightShaderModels.push_back(donut);
    lightShaderModels.push_back(cube);
    lightShaderModels.push_back(sphere);
    
    Model skybox = Model("skybox", false, 0, 0, 0, 1000);
    noLightShaderModels.push_back(skybox);
    
    Model sun = Model("light1", false, 0, 0, 0, 1);
    Model moon = Model("light1", false, 0, 0, 0, 1);
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        clearScreen();
        
        // Update camera + v
        updateCameraPosition();
        updateCameraRotation();
        glm::mat4 v = getV();
        
        // Update time
        if (pause) {
            glfwSetTime(timeValue);
        } else {
            timeValue = glfwGetTime();
        }
        // Day / night cycle
        glm::vec3 sunPos = DayNightCycle::getSunPos(timeValue);
        glm::vec3 sunColor = DayNightCycle::getSunColor(timeValue);
        sun.updateM(sunPos.x, sunPos.y, sunPos.z, 100.0);
        glm::vec3 moonPos = DayNightCycle::getMoonPos(timeValue);
        glm::vec3 moonColor = DayNightCycle::getMoonColor(timeValue);
        moon.updateM(moonPos.x, moonPos.y, moonPos.z, 100.0);
        
        lightSourceShader.use();
        // Sun
        lightSourceShader.setMatrix4("mvp", p * v * sun.m);
        lightSourceShader.setVector3f("lightColor", sunColor.x, sunColor.y, sunColor.z);
        sun.Draw(lightSourceShader);
        // Moon
        lightSourceShader.setMatrix4("mvp", p * v * moon.m);
        lightSourceShader.setVector3f("lightColor", moonColor.x, moonColor.y, moonColor.z);
        moon.Draw(lightSourceShader);
        
        
        glm::vec3 lightPos;
        glm::vec3 lightColor;
        if (sunPos.y > moonPos.y) {
            lightPos = sunPos;
            lightColor = sunColor;
        } else {
            lightPos = moonPos;
            lightColor = moonColor;
        }
        
        
        // Models without lighting
        noLightShader.use();
        noLightShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        for (Model model : noLightShaderModels) {
            noLightShader.setMatrix4("mvp", p * v * model.m);
            glBindTexture(GL_TEXTURE_2D, model.texture);
            model.Draw(noLightShader);
        }
        // Models with lighting
        lightShader.use();
        lightShader.setVector3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
        lightShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        lightShader.setMatrix4("v", v);
        for (Model model : lightShaderModels) {
            lightShader.setMatrix4("mvp", p * v * model.m);
            lightShader.setMatrix4("m", model.m);
            lightShader.setFloat("ns", model.ns);
            glBindTexture(GL_TEXTURE_2D, model.texture);
            model.Draw(lightShader);
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



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
#define SKYBOX_VERT_PATH "skybox.vert"
#define SKYBOX_FRAG_PATH "skybox.frag"

glm::mat4 getVSkybox();
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
    
    Shader skyboxShader = Shader(SKYBOX_VERT_PATH, SKYBOX_FRAG_PATH, NULL, NULL, NULL);
    skyboxShader.compile();
    
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

    Model sun = Model("light1", false, 0, 0, 0, 1);
    Model moon = Model("light1", false, 0, 0, 0, 1);

    
    Model block = Model("block", true, 3, 10, 1, 0.02, 5);
	std::vector<glm::mat4> block_positions;
	for (float i = 0.0f; i < 96.0f; i += 4.0f) {
		for (float j = 0.0f; j < 96.0f; j += 4.0f) {
			glm::mat4 position = cube.getM(j, -10, i, 2.0f);
			block_positions.push_back(position);
		}
	}
	
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();
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
        lightSourceShader.setVector3f("lightColor", 3*moonColor.x, 3*moonColor.y, 3*moonColor.z);
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
        
        // Models with lighting
        lightShader.use();
        lightShader.setVector3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
        lightShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        lightShader.setMatrix4("v", v);
        
        Model model = block;
        for (float i = 0.0f; i < 24.0f; i += 1.0f) {
            for (float j = 0.0f; j < 24.0f; j += 1.0f) {
                lightShader.setMatrix4("mvp", p * v * block_positions[j + 24*i]);
                lightShader.setMatrix4("m", block_positions[j + 24 * i]);
                lightShader.setFloat("ns", model.ns);
                model.Draw(lightShader);
            }
        }
        for (Model model : lightShaderModels) {
            lightShader.setMatrix4("mvp", p * v * model.m);
            lightShader.setMatrix4("m", model.m);
            lightShader.setFloat("ns", model.ns);
            model.Draw(lightShader);
        }
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        skyboxShader.setMatrix4("mvp", p * getVSkybox() * skybox.m);
        skybox.Draw(skyboxShader);

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

glm::mat4 getVSkybox() {
    return glm::lookAt(
    glm::vec3(0, 0, 0),           // Position
    direction, // and looks at
    up  // Head is up (set to 0,-1,0 to look upside-down)
    );
}



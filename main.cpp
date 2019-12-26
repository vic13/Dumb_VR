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

// Def
using std::cout;
using std::endl;
#define LIGHT_SOURCE_VERT_PATH "light_source.vert"
#define LIGHT_SOURCE_FRAG_PATH "light_source.frag"
#define LIGHT_VERT_PATH "with_light.vert"
#define LIGHT_FRAG_PATH "with_light.frag"
#define SKYBOX_VERT_PATH "skybox.vert"
#define SKYBOX_FRAG_PATH "skybox.frag"
#define BUMP_VERT_PATH "bump_map.vert"
#define BUMP_FRAG_PATH "bump_map.frag"

glm::mat4 getV(bool skybox = false);
glm::mat4 getP();

bool dayNightCycle = true;

int main() {
    GLFWwindow* mWindow = init_gl();
    float timeValue = glfwGetTime();
    
    //Enable blending (alpha)
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // Shaders
    Shader lightShader = Shader(LIGHT_VERT_PATH, LIGHT_FRAG_PATH, NULL, NULL, NULL);
    lightShader.compile();
    std::vector<Model*> lightShaderModels;
    
    Shader skyboxShader = Shader(SKYBOX_VERT_PATH, SKYBOX_FRAG_PATH, NULL, NULL, NULL);
    skyboxShader.compile();
    
    Shader lightSourceShader = Shader(LIGHT_SOURCE_VERT_PATH, LIGHT_SOURCE_FRAG_PATH, NULL, NULL, NULL);
    lightSourceShader.compile();
    
    Shader bumpShader = Shader(BUMP_VERT_PATH, BUMP_FRAG_PATH, NULL, NULL, NULL);
    bumpShader.compile();
    
    // Matrices
    glm::mat4 p = getP();
    
    // Models
    Model donut = Model("donut", true, 3, 0, 0, 5, 2);
    Model cube = Model("cube", true, 3, 0, 1, 0.2, 5);
    Model sphere = Model("smoothSphere", true, 3, 0, -1, 4, 20);
    lightShaderModels.push_back(&donut);
    lightShaderModels.push_back(&cube);
    lightShaderModels.push_back(&sphere);
    
    Model bumpCube = Model("bump_cube", true, 6, 0, 0, 0.5, 5, true, false);
    
    Model skybox = Model("skybox", false, 0, 0, 0, 1000);

    Model sun = Model("light1", false, 0, 0, 0, 100);
    Model moon = Model("light1", false, 0, 0, 0, 100);
    
    Model steve = Model("cube", true, 0, 0, 0, 0.1, 5);

    /*
    Model block = Model("block", true, 3, 10, 1, 0.02, 5);
	std::vector<glm::mat4> block_positions;
	for (float i = 0.0f; i < 96.0f*3; i += 4.0f) {
		for (float j = 0.0f; j < 96.0f*3; j += 4.0f) {
			glm::mat4 position = cube.getM(j, -10, i, 2.0f);
			block_positions.push_back(position);
		}
	}*/
	
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();
        clearScreen();

       
        // Update camera + v
        updateFirstPerson();
        updateStevePosition();
        updateCameraRotation();
        
        glm::mat4 v = getV();
        glm::vec3 camPos;
        if (firstPerson) {
            camPos = stevePos;
        } else {
            camPos = stevePos-direction;
        }
        
        
        // Update time
        if (pause) {
            glfwSetTime(timeValue);
        } else {
            timeValue = glfwGetTime();
        }
        
        glm::vec3 lightPos = glm::vec3(0, 10, 0);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        // Day / night cycle
        if (dayNightCycle) {
            glm::vec3 sunPos = DayNightCycle::getSunPos(timeValue);
            glm::vec3 sunColor = DayNightCycle::getSunColor(timeValue);
            sun.updatePosition(sunPos.x, sunPos.y, sunPos.z);
            glm::vec3 moonPos = DayNightCycle::getMoonPos(timeValue);
            glm::vec3 moonColor = DayNightCycle::getMoonColor(timeValue);
            moon.updatePosition(moonPos.x, moonPos.y, moonPos.z);
            
            lightSourceShader.use();
            // Sun
            lightSourceShader.setMatrix4("mvp", p * v * sun.m);
            lightSourceShader.setVector3f("lightColor", sunColor.x, sunColor.y, sunColor.z);
            sun.Draw(lightSourceShader);
            // Moon
            lightSourceShader.setMatrix4("mvp", p * v * moon.m);
            lightSourceShader.setVector3f("lightColor", 3*moonColor.x, 3*moonColor.y, 3*moonColor.z);
            moon.Draw(lightSourceShader);
            
            
            if (sunPos.y > moonPos.y) {
                lightPos = sunPos;
                lightColor = sunColor;
            } else {
                lightPos = moonPos;
                lightColor = moonColor;
            }
        }
        
        
        // Models with lighting
        lightShader.use();
        lightShader.setVector3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
        lightShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        lightShader.setMatrix4("v", v);
        /*
        Model model = block;
        lightShader.setMatrix4("mvp", p * v * block_positions[0]);
        lightShader.setMatrix4("m", block_positions[0]);
        lightShader.setFloat("ns", model.ns);
        model.Draw(lightShader);
        for (float i = 0.0f; i < 24.0f*3; i += 1.0f) {
            for (float j = 0.0f; j < 24.0f*3; j += 1.0f) {
                lightShader.setMatrix4("mvp", p * v * block_positions[j + 3*24*i]);
                lightShader.setMatrix4("m", block_positions[j + 3*24 * i]);
                lightShader.setFloat("ns", model.ns);
                model.DrawMultiple();
                //model.Draw(lightShader);
            }
        }*/
        cube.updateRotation(timeValue, glm::vec3(1, 0, 0));
        for (Model* modelPointer : lightShaderModels) {
            Model model = *modelPointer;
            lightShader.setMatrix4("mvp", p * v * model.m);
            lightShader.setMatrix4("m", model.m);
            lightShader.setFloat("ns", model.ns);
            model.Draw(lightShader);
        }
        
        steve.updatePosition(stevePos.x, stevePos.y, stevePos.z);
        if (!firstPerson) {
            lightShader.setMatrix4("mvp", p * v * steve.m);
            lightShader.setMatrix4("m", steve.m);
            lightShader.setFloat("ns", steve.ns);
            steve.Draw(lightShader);
        }
        
        
        // Bump map
        bumpCube.updateRotation(timeValue, glm::vec3(1, 1, 1));
        bumpShader.use();
        bumpShader.setVector3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
        bumpShader.setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        bumpShader.setMatrix4("mvp", p * v * bumpCube.m);
        bumpShader.setMatrix4("m", bumpCube.m);
        bumpShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        bumpShader.setFloat("ns", bumpCube.ns);
        bumpCube.Draw(bumpShader);
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        skyboxShader.setMatrix4("mvp", p * getV(true) * skybox.m);
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

glm::mat4 getV(bool skybox) {
    glm::vec3 newStevePos = stevePos;
    if (skybox) {
        newStevePos = glm::vec3(0,0,0);
    }
    glm::vec3 lookAt;
    glm::vec3 camPos;
    if (firstPerson) {
        camPos = newStevePos;
        lookAt = camPos+direction;
    } else {
        camPos = newStevePos-direction;
        lookAt = newStevePos;
    }
    
    return glm::lookAt(camPos, lookAt, up); // Head is up (set to 0,-1,0 to look upside-down)
}



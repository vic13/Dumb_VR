// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"
#include "Model.hpp"
#include "DayNightCycle.hpp"
#include "chunk.hpp"

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
#include <string>

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
#define BLOCK_VERT_PATH "block_shader.vert"
#define BLOCK_FRAG_PATH "block_shader.frag"

glm::mat4 getV(bool skybox = false);
glm::mat4 getP();
void setModelUniforms(Shader shader, Model* model, glm::mat4 p, glm::mat4 v);

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
    
    Shader bumpShader = Shader(BUMP_VERT_PATH, LIGHT_FRAG_PATH, NULL, NULL, NULL);
    bumpShader.compile();
    std::vector<Model*> bumpShaderModels;
    
    Shader skyboxShader = Shader(SKYBOX_VERT_PATH, SKYBOX_FRAG_PATH, NULL, NULL, NULL);
    skyboxShader.compile();
    
    Shader lightSourceShader = Shader(LIGHT_SOURCE_VERT_PATH, LIGHT_SOURCE_FRAG_PATH, NULL, NULL, NULL);
    lightSourceShader.compile();
    
	Shader chunkShader = Shader(BLOCK_VERT_PATH, BLOCK_FRAG_PATH, NULL, NULL, NULL);
	chunkShader.compile();

    // Matrices
    glm::mat4 p = getP();
    
    // Models
    Model donut = Model("donut", true, 3, 0, 0, 5, 5);
    Model cube = Model("cube", true, 3, 0, 1, 0.2, 5);
    Model sphere = Model("smoothSphere", true, 3, 0, -1, 4, 20);
    lightShaderModels.push_back(&donut);
    lightShaderModels.push_back(&cube);
    lightShaderModels.push_back(&sphere);
    
    Model bumpCube = Model("bump_cube", true, 6, 0, 0, 0.5, 5, true, false);
    Model bumpCube2 = Model("bump_cube", true, 8, 0, 0, 0.7, 5, true, false);
    bumpShaderModels.push_back(&bumpCube);
    bumpShaderModels.push_back(&bumpCube2);
    
    Model skybox = Model("skybox", false, 0, 0, 0, 1000);

    Model sun = Model("light1", false, 0, 0, 0, 100);
    Model moon = Model("light1", false, 0, 0, 0, 100);
    
    Model steve = Model("steve", true, 0, 0, 0, 0.1, 5);


	Chunk chunk = Chunk();
    int chunkSide = 10;


    /*
    Model block = Model("block", true, 3, 10, 1, 0.02, 5);
	std::vector<glm::mat4> block_positions;
	for (float i = 0.0f; i < 96.0f*3; i += 4.0f) {
		for (float j = 0.0f; j < 96.0f*3; j += 4.0f) {
			glm::mat4 position = cube.getM(j, -10, i, 2.0f);
			block_positions.push_back(position);
		}
	}*/
	
    GLuint flashlight_tex = createTexture("VR_Assets/flashlight.png", true);
    
    std::vector<glm::vec3> pointLightPositions;
    pointLightPositions.push_back(glm::vec3( 0.7f,  0.2f,  2.0f));
    pointLightPositions.push_back(glm::vec3( 2.3f, -3.3f, -4.0f));
    pointLightPositions.push_back(glm::vec3(-4.0f,  2.0f, -12.0f));
    pointLightPositions.push_back(glm::vec3( 0.0f,  0.0f, -3.0f));
    std::vector<glm::vec3> pointLightColors;
    pointLightColors.push_back(glm::vec3( 1.0f,  0.0f,  0.0f));
    pointLightColors.push_back(glm::vec3( 0.0f,  1.0f,  0.0f));
    pointLightColors.push_back(glm::vec3( 0.0f,  0.0f,  1.0f));
    pointLightColors.push_back(glm::vec3( 1.0f,  0.0f,  1.0f));
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();
        clearScreen();

       
        // Update camera + v
        updateFirstPerson();
        updateStevePosition();
        updateCameraRotation();
        updateFlashLight();
        
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
        
        // Models with lighting
        cube.updateRotation(timeValue, glm::vec3(1, 0, 0));
        lightShader.use();
        lightShader.setUniforms(stevePos, direction, right, camPos, lightColor, flashlightOn, flashlight_tex, pointLightPositions, pointLightColors, false);
        for (Model* modelPointer : lightShaderModels) {
            setModelUniforms(lightShader, modelPointer, p, v);
            modelPointer->Draw(lightShader);
        }
        steve.updatePosition(stevePos.x, stevePos.y, stevePos.z);
        steve.updateRotation(hAngle-M_PI/2.0, glm::vec3(0, 1, 0));
        if (!firstPerson) {
            setModelUniforms(lightShader, &steve, p, v);
            steve.Draw(lightShader);
        }
        
        
        // Bump map
        bumpCube.updateRotation(timeValue, glm::vec3(1, 1, 1));
        
        bumpShader.use();
        bumpShader.setUniforms(stevePos, direction, right, camPos, lightColor, flashlightOn, flashlight_tex, pointLightPositions, pointLightColors, true);
        for (Model* modelPointer : bumpShaderModels) {
            setModelUniforms(bumpShader, modelPointer, p, v);
            modelPointer->Draw(bumpShader);
        }
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        skyboxShader.setMatrix4("mvp", p * getV(true) * skybox.m);
        skybox.Draw(skyboxShader);

		//Chunk
		chunkShader.use();
        chunkShader.setVector3f("pointlightPosition", lightPos.x, lightPos.y, lightPos.z);
        chunkShader.setVector3f("flashlightPosition", stevePos.x, stevePos.y, stevePos.z);
        chunkShader.setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        chunkShader.setMatrix4("v", v);
        chunkShader.setVector3f("pointlightColor", lightColor.x, lightColor.y, lightColor.z);
        chunkShader.setVector3f("flashlight.color", 1.0f, 1.0f, 1.0f); // purple
        chunkShader.setVector3f("flashlight.direction", direction.x, direction.y, direction.z);
        chunkShader.setFloat("flashlight.cosAngle", cos(M_PI / 9.0)); // 20°
        chunkShader.setInteger("flashlight.on", flashlightOn);
        chunkShader.setVector3f("right", right.x, right.y, right.z);
        chunkShader.setFloat("ns", bumpCube.ns);
        int i = 10;
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(lightShader.ID, "texture_flashlight"), i);
        glBindTexture(GL_TEXTURE_2D, flashlight_tex);
        
		for (int j = 16; j < chunkSide * 16; j += 16) {
			for (int t = 0; t < chunkSide * 16; t += 16) {
				chunkShader.setMatrix4("m", cube.getM(6 + t, 2, j, 1.0f));
				chunkShader.setMatrix4("mvp", p* v* cube.getM(6+t, 2, j, 1.0f));
				chunk.render(chunkShader);
			}
		}

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}


void setModelUniforms(Shader shader, Model* model, glm::mat4 p, glm::mat4 v) {
    shader.setMatrix4("mvp", p * v * model->m);
    shader.setMatrix4("m", model->m);
    shader.setFloat("ns", model->ns);
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

void setupShader() {
    
}

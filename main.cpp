// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"
#include "Model.hpp"
#include "DayNightCycle.hpp"
#include "Chunk.hpp"
#include "PointLight.hpp"

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
#define BLOCK_VERT_PATH "chunk.vert"

glm::mat4 getV(bool skybox = false);
glm::mat4 getP();
void setModelUniforms(Shader shader, Model* model, glm::mat4 pv);

bool dayNightCycle = true;
unsigned int maxNbTorchs = 10;

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
    std::vector<Model*> lightSourceShaderModels;
    
	Shader chunkShader = Shader(BLOCK_VERT_PATH, LIGHT_FRAG_PATH, NULL, NULL, NULL);
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
    lightSourceShaderModels.push_back(&sun);
    lightSourceShaderModels.push_back(&moon);
    Model torchModel = Model("light1", false, 0, 0, 0, 1);
    glm::vec3 torchColor(1.0f,  0.5f,  0.0f);
    
    Model steve = Model("steve", true, 0, 0, 0, 0.1, 5);


	std::vector<Chunk *> chunks;
	int chunkSide = 30;
	for (int j = 16; j < chunkSide * 16; j += 16) {
		for (int t = 0; t < chunkSide * 16; t += 16) {
			chunks.push_back(new Chunk(6+t, 2, j, 1.0f));
		}
	}
    
    int flashlightTextureSlot = 10;	
    GLuint flashlight_tex = createTexture("VR_Assets/flashlight.png", true);
    
    std::vector<PointLight> torchs;

	chunkShader.use();
	glActiveTexture(GL_TEXTURE0 + flashlightTextureSlot);
	glUniform1i(glGetUniformLocation(lightShader.ID, "texture_flashlight"), flashlightTextureSlot);
	glBindTexture(GL_TEXTURE_2D, flashlight_tex);


	lightShader.use(); // Just send texture once 
	glActiveTexture(GL_TEXTURE0 + flashlightTextureSlot);
	glUniform1i(glGetUniformLocation(lightShader.ID, "texture_flashlight"), flashlightTextureSlot);
	glBindTexture(GL_TEXTURE_2D, flashlight_tex);
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		showFPS();
        clearScreen();

       
        // Update camera + v
        updateFirstPerson();
        updateStevePosition();
        updateCameraRotation();
        updateFlashLight();
        if (torchs.size() < maxNbTorchs) {
            if (addLight()) {
                PointLight l = {stevePos+0.5f*direction, torchColor};
                torchs.push_back(l);
            }
        }
        
        glm::mat4 v = getV();
		glm::mat4 pv = p * v;

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
        
        glm::vec3 directionalLightL;
        glm::vec3 directionalLightColor;
        glm::vec3 sunPos = DayNightCycle::getSunPos(timeValue);
        sun.updatePosition(sunPos.x, sunPos.y, sunPos.z);
        sun.color = DayNightCycle::getSunColor(timeValue);
        
        glm::vec3 moonPos = DayNightCycle::getMoonPos(timeValue);
        moon.updatePosition(moonPos.x, moonPos.y, moonPos.z);
        moon.color = DayNightCycle::getMoonColor(timeValue);
        
        lightSourceShader.use();
        for (Model* modelPointer : lightSourceShaderModels) {
            lightSourceShader.setMatrix4("mvp", pv * modelPointer->m);
            lightSourceShader.setVector3f("lightColor", modelPointer->color.x, modelPointer->color.y, modelPointer->color.z);
            modelPointer->Draw(lightSourceShader);
        }
        lightSourceShader.setVector3f("lightColor", torchColor.x, torchColor.y, torchColor.z);
        for (PointLight torch : torchs) {
            torchModel.updatePosition(torch.position.x, torch.position.y, torch.position.z);
            lightSourceShader.setMatrix4("mvp", pv * torchModel.m);
            torchModel.Draw(lightSourceShader);
        }
        
        
        if (sunPos.y > moonPos.y) {
            directionalLightL = sunPos;
            directionalLightColor = sun.color;
        } else {
            directionalLightL = moonPos;
            directionalLightColor = moon.color;
        }
        
        // Models with lighting
        cube.updateRotation(timeValue, glm::vec3(1, 0, 0));
        lightShader.use();
        lightShader.setUniforms(stevePos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, false);
        for (Model* modelPointer : lightShaderModels) {
            setModelUniforms(lightShader, modelPointer, pv);
            modelPointer->Draw(lightShader);
        }

        steve.updatePosition(stevePos.x, stevePos.y, stevePos.z);
        if (!firstPerson) {
            setModelUniforms(lightShader, &steve, pv);
            steve.Draw(lightShader);
        }
        
        
        // Bump map
        bumpCube.updateRotation(timeValue, glm::vec3(1, 1, 1));
        bumpShader.setUniforms(stevePos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, true);
        for (Model* modelPointer : bumpShaderModels) {
            setModelUniforms(bumpShader, modelPointer, pv);
            modelPointer->Draw(bumpShader);
        }
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setVector3f("lightColor", directionalLightColor.x, directionalLightColor.y, directionalLightColor.z);
        skyboxShader.setMatrix4("mvp", p * getV(true) * skybox.m);
        skybox.Draw(skyboxShader);

		//Chunk
        chunkShader.use();
        chunkShader.setUniforms(stevePos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, false, true);
        chunkShader.setMatrix4("m", chunks[0]->getChunkModel());
        chunkShader.setMatrix4("mvp", pv * chunks[0]->getChunkModel());
        chunkShader.setFloat("ns", bumpCube.ns);
        chunks[0]->render(chunkShader);
        
        for (unsigned int j = 1; j < chunks.size(); j++) {
            chunkShader.setMatrix4("m", chunks[j]->getChunkModel());
            chunkShader.setMatrix4("mvp", pv * chunks[j]->getChunkModel());
            chunks[j]->renderMultiple();
        }

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}


void setModelUniforms(Shader shader, Model* model, glm::mat4 pv) {
    shader.setMatrix4("mvp", pv * model->m);
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

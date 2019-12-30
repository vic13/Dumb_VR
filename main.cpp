// Local Headers
#include "glitter.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "controls.hpp"
#include "Model.hpp"
#include "DayNightCycle.hpp"
#include "Chunk.hpp"
#include "PointLight.hpp"
#include "World.hpp"
#include "ParticleSource.hpp"

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
void setModelUniforms(Shader shader, glm::mat4 m, glm::mat4 pv, Material material);

bool dayNightCycle = true;
unsigned int maxNbTorchs = 10;
glm::vec3 steveHeight = glm::vec3(0, 2, 0);

int main() {
    srand(time(NULL));
    
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
    Model donut = Model("donut", true, glm::vec3(15, 0, 0), 50, {0.3, 0.6, 0, 5});
    Model cube = Model("cube", true, glm::vec3(15, 0, -15), 2, {0.3, 0.3, 0.4, 20});
    Model sphere = Model("smoothSphere", true, glm::vec3(30, 0, 0), 40, {0.3, 0.3, 0.4, 20});
    lightShaderModels.push_back(&donut);
    lightShaderModels.push_back(&cube);
    lightShaderModels.push_back(&sphere);
    
    Model bumpCube = Model("bump_cube", true, glm::vec3(30, 0, -15), 5, {0.3, 0.4, 0.3, 5}, true, false);
    Model bumpCube2 = Model("bump_cube", true, glm::vec3(45, 0, 0), 5, {0.3, 0.4, 0.3, 5}, true, false);
    bumpShaderModels.push_back(&bumpCube);
    bumpShaderModels.push_back(&bumpCube2);
    
    Model skybox = Model("skybox", false, glm::vec3(0, 0, 0), 1000);

    Model sun = Model("light1", false, glm::vec3(0, 0, 0), 100);
    Model moon = Model("light1", false, glm::vec3(0, 0, 0), 100);
    lightSourceShaderModels.push_back(&sun);
    lightSourceShaderModels.push_back(&moon);
    Model torchModel = Model("torch", true, glm::vec3(0, 0, 0), 1, {0.3, 0.4, 0.3, 5});
    glm::vec3 torchColor(1.0f,  0.5f,  0.0f);
    glm::vec3 torchHeight(0,0.9,0);
    
    Model particleModel = Model("particle", false, glm::vec3(3, 0, 1), 0.02);
    
    Model steve = Model("steve", true, glm::vec3(0, 0, 0), 0.8, {0.4, 0.5, 0.1, 5});

    Material chunkMaterial = {0.3, 0.3, 0.3, 5};
    World world = World(5, chunkShader);  // Change Map Size here
    
    GLuint flashlight_tex = createTexture("VR_Assets/flashlight.png", true);
    
    std::vector<PointLight> torchs;
    std::vector<ParticleSource> particleSources;
    
    int flashlightTextureSlot = 10;
	chunkShader.use(); // Just send texture once
	glActiveTexture(GL_TEXTURE0 + flashlightTextureSlot);
	glUniform1i(glGetUniformLocation(lightShader.ID, "texture_flashlight"), flashlightTextureSlot);
	glBindTexture(GL_TEXTURE_2D, flashlight_tex);
	lightShader.use(); // Just send texture once 
	glActiveTexture(GL_TEXTURE0 + flashlightTextureSlot);
	glUniform1i(glGetUniformLocation(lightShader.ID, "texture_flashlight"), flashlightTextureSlot);
	glBindTexture(GL_TEXTURE_2D, flashlight_tex);
    bumpShader.use(); // Just send texture once
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
        glm::vec3 flashlightPos = stevePos+(steveHeight*0.7f);
        if (torchs.size() < maxNbTorchs) {
            if (addLight()) {
                glm::vec3 position = stevePos+steveHeight+2.0f*direction+torchHeight;
                PointLight l = {position, torchColor};
                torchs.push_back(l);
                ParticleSource ps = ParticleSource(position, torchColor, glm::vec3(0.5f, 0.5f, 0.5f));
                particleSources.push_back(ps);
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
        sun.updatePosition(sunPos);
        sun.color = DayNightCycle::getSunColor(timeValue);
        
        glm::vec3 moonPos = DayNightCycle::getMoonPos(timeValue);
        moon.updatePosition(moonPos);
        moon.color = DayNightCycle::getMoonColor(timeValue);
        
        // Light sources
        lightSourceShader.use();
        for (Model* modelPointer : lightSourceShaderModels) {
            lightSourceShader.setMatrix4("mvp", pv * modelPointer->m);
            lightSourceShader.setVector3f("lightColor", 2*modelPointer->color.x, 2*modelPointer->color.y, 2*modelPointer->color.z);
            modelPointer->Draw(lightSourceShader);
        }
        for (unsigned int i = 0; i<particleSources.size(); i++) {
            particleSources[i].updateParticles();
            for (Particle p : particleSources[i].getParticles()) {
                particleModel.updatePosition(p.position);
                float cosTheta = glm::acos(glm::dot(planeDirection, glm::vec3(1,0,0)));
                if (planeDirection.z > 0) {
                    cosTheta *= -1;
                }
                particleModel.updateRotation(cosTheta, glm::vec3(0,1,0));
                lightSourceShader.setVector3f("lightColor", p.color.x, p.color.y, p.color.z);
                
                lightSourceShader.setMatrix4("mvp", pv * particleModel.m);
                particleModel.Draw(lightSourceShader);
            }
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
        lightShader.setUniforms(flashlightPos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, false);
        for (Model* modelPointer : lightShaderModels) {
            setModelUniforms(lightShader, modelPointer->m, pv, modelPointer->material);
            modelPointer->Draw(lightShader);
        }

        steve.updatePosition(stevePos);
        if (!firstPerson) {
            setModelUniforms(lightShader, steve.m, pv, steve.material);
            steve.Draw(lightShader);
        }
        
        for (PointLight torch : torchs) {
            torchModel.updatePosition(torch.position-torchHeight);
            setModelUniforms(lightShader, torchModel.m, pv, torchModel.material);
            torchModel.Draw(lightSourceShader);
        }
        
        
        // Bump map
        bumpCube.updateRotation(timeValue, glm::vec3(1, 1, 1));
        bumpShader.use();
        bumpShader.setUniforms(flashlightPos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, true);
        for (Model* modelPointer : bumpShaderModels) {
            setModelUniforms(bumpShader, modelPointer->m, pv, modelPointer->material);
            modelPointer->Draw(bumpShader);
        }
        
        // Skybox
        skyboxShader.use();
        skyboxShader.setVector3f("lightColor", directionalLightColor.x, directionalLightColor.y, directionalLightColor.z);
        skyboxShader.setMatrix4("mvp", p * getV(true) * skybox.m);
        skybox.Draw(skyboxShader);

		//Chunk
        chunkShader.use();
        chunkShader.setUniforms(flashlightPos, flashlightDirection, right, camPos, flashlightOn, flashlight_tex, torchs, directionalLightL, directionalLightColor, false, true);
        world.render(pv, chunkMaterial);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
	glfwTerminate();
    return EXIT_SUCCESS;
}


void setModelUniforms(Shader shader, glm::mat4 m, glm::mat4 pv, Material material) {
    shader.setMatrix4("mvp", pv * m);
    shader.setMatrix4("m", m);
    shader.setFloat("material.ns", material.ns);
    shader.setFloat("material.ka", material.ka);
    shader.setFloat("material.kd", material.kd);
    shader.setFloat("material.ks", material.ks);
}


glm::mat4 getP() {
    // Projection matrix : 45° Field of View, width:height ratio, display range : 0.1 unit <-> 4000 units
    return glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 4000.0f);
      
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
}

glm::mat4 getV(bool skybox) {
    glm::vec3 steveHeadPos = stevePos+steveHeight;
    if (skybox) {
        steveHeadPos = glm::vec3(0,0,0);
    }
    glm::vec3 lookAt;
    glm::vec3 camPos;
    if (firstPerson) {
        camPos = steveHeadPos;
        lookAt = camPos+direction;
    } else {
        camPos = steveHeadPos-5.0f*direction;
        lookAt = steveHeadPos;
    }
    
    return glm::lookAt(camPos, lookAt, up); // Head is up (set to 0,-1,0 to look upside-down)
}

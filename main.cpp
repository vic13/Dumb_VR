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
glm::vec3 getLightPos(float timeValue);

// light center + radius + period
float lightX = 3.0;
float lightY = 0.0;
float lightZ = 1.0;
float lightRadius = 3.0;
float lightPeriod = 3.0;

int main() {
    GLFWwindow* mWindow = init_gl();
    float timeValue = glfwGetTime();
    
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
    Model donut = Model("donut", true, 3, 0, 0, 5, 2);
    Model cube = Model("cube", true, 3, 0, 1, 0.2, 5);
    lightShaderModels.push_back(donut);
    lightShaderModels.push_back(cube);
    
    Model light1 = Model("light1", false, 0, 0, 0, 1);
    Model skybox = Model("skybox", false, 0, 0, 0, 1000);
    noLightShaderModels.push_back(light1);
    noLightShaderModels.push_back(skybox);
    
    
    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
		//showFPS();
        clearScreen();
        
        // Update camera + v
        updateCameraPosition();
        updateCameraRotation();
        glm::mat4 v = getV();
        
        // Update light position
        if (pause) {
            glfwSetTime(timeValue);
        } else {
            timeValue = glfwGetTime();
        }
        glm::vec3 lightPos = getLightPos(timeValue);
        
        // Models without lighting
        noLightShader.use();
        noLightShaderModels.front().updateM(lightPos.x, lightPos.y, lightPos.z, 1.0);
        for (Model model : noLightShaderModels) {
            noLightShader.setMatrix4("mvp", p * v * model.m);
            glBindTexture(GL_TEXTURE_2D, model.texture);
            model.Draw(noLightShader);
        }

        // Models with lighting
        lightShader.use();
        lightShader.setVector3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
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

glm::vec3 getLightPos(float timeValue) {
    float posX = lightX + lightRadius*sin(timeValue*2*3.14/lightPeriod);
    float posY = lightY;
    float posZ = lightZ + lightRadius*cos(timeValue*2*3.14/lightPeriod);
    return glm::vec3(posX, posY, posZ);
}

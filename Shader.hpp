#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// Simple shader class from http://www.learnopengl.com/ with a few tweaks
class Shader {
public:
	// State
	GLuint ID;

	// Constructor
	Shader(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr, const GLchar *tessCPath = nullptr, const GLchar *tessEPath = nullptr);

	// Sets the current shader as active, do we need to return?
	Shader& use();

	// Not sure compile should be it's own step separate from constructor
	void compile();

	void setFloat(const GLchar *name, GLfloat value);
	void setInteger(const GLchar *name, GLint value);

	void setVector2f(const GLchar *name, GLfloat x, GLfloat y);
	void setVector2f(const GLchar *name, const glm::vec2 &value);

	void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
	void setVector3f(const GLchar *name, const glm::vec3 &value);

	void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void setVector4f(const GLchar *name, const glm::vec4 &value);

	void setMatrix4(const GLchar *name, const glm::mat4 &matrix);
    
    void setUniforms(glm::vec3 stevePos, glm::vec3 direction, glm::vec3 right, glm::vec3 camPos, glm::vec3 lightColor, bool flashlightOn, GLuint flashlight_tex, std::vector<glm::vec3> pointLightPositions, std::vector<glm::vec3> pointLightColors, bool bumpMapping) {
        setVector3f("flashlightPosition", stevePos.x, stevePos.y, stevePos.z);
        setVector3f("flashlightDirection", direction.x, direction.y, direction.z);
        setVector3f("cameraPosition", camPos.x, camPos.y, camPos.z);
        setInteger("bump_mapping", bumpMapping);
        setVector3f("lightColor", lightColor.x, lightColor.y, lightColor.z);
        setVector3f("flashlight.color", 1.0f, 1.0f, 1.0f);
        setFloat("flashlight.cosAngle", cos(M_PI/9.0)); // 20°
        setInteger("flashlight.on", flashlightOn);
        setVector3f("right", right.x, right.y, right.z);
        for (unsigned int i=0; i<pointLightPositions.size(); i++) {
            glm::vec3 position = pointLightPositions[i];
            glm::vec3 color = pointLightColors[i];
            std::string namePosition = "pointLights["+std::to_string(i)+"].position";
            setVector3f(namePosition.c_str(), position.x, position.y, position.z);
            std::string nameColor = "pointlightColors["+std::to_string(i)+"]";
            setVector3f(nameColor.c_str(), color.x, color.y, color.z);
        }
        int i = 10;
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(ID, "texture_flashlight"), i);
        glBindTexture(GL_TEXTURE_2D, flashlight_tex);
    }
    
private:
	// Checks if compilation or linking failed and if so, print the error logs
	void checkCompileErrors(const GLuint &object, std::string type);

	// Make a shader from a filepath
	GLuint pathToShader(const GLchar * path, GLenum shaderType);

	// Get a string of the shader type from the GLenum
	std::string shaderTypeToString(GLenum shaderType);

	const GLchar* mVertexPath;
	const GLchar* mFragmentPath;
	const GLchar* mGeometryPath;
	const GLchar* mTessCPath;
	const GLchar* mTessEPath;
};

#endif

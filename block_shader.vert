#version 330 core

layout(location=0) in vec4 coord;
layout(location=1) in vec3 normal;


out vec4 texcoord;
out vec3 L_pointlight;
out vec3 L_flashlight;
out vec3 N;
out vec3 V;
out vec3 test;

uniform mat4 mvp;
uniform vec3 pointlightPosition;
uniform vec3 flashlightPosition;
uniform vec3 cameraPosition;
uniform mat4 m;

void main(void) {
	gl_Position = mvp * vec4(coord.xyz, 1);
    texcoord = coord;

	vec3 position = coord.xyz;

	vec3 position_worldCoord = (m * vec4(position,1) ).xyz;
	test = position_worldCoord;

	L_pointlight = normalize(pointlightPosition-position_worldCoord);   // unit vector vertex -> point light
	L_flashlight = normalize(flashlightPosition-position_worldCoord);  // unit vector vertex -> flashlight
	vec3 normal_worldCoord = (m*vec4(normal, 0)).xyz; // takes into account rotation of the model
    N = normalize(normal_worldCoord);
    V = normalize(cameraPosition-position_worldCoord); //unit vector vertex -> camera
        
}
#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;

out vec2 uv;
out vec3 L_pointlight;
out vec3 L_flashlight;
out vec3 N;
out vec3 V;

uniform vec3 pointlightPosition;
uniform vec3 flashlightPosition;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;


void main() {
    gl_Position = mvp * vec4(position, 1);
    uv = vertexUV;
    
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    L_pointlight = pointlightPosition-position_worldCoord;   // vector vertex -> point light
    L_flashlight = flashlightPosition-position_worldCoord;  // if normalized : bad results with interpolation
    vec3 normal_worldCoord = (m*vec4(normal,0)).xyz; // takes into account rotation of the model
    N = normal_worldCoord;
    V = cameraPosition-position_worldCoord; //unit vector vertex -> camera
    
    
}

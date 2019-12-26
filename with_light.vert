#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;

out vec2 uv;
out vec3 N;
out vec3 L;
out vec3 V;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

void main() {
    gl_Position = mvp * vec4(position, 1);
    uv = vertexUV;
    
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    L = normalize(lightPosition-position_worldCoord); // unit vector vertex -> light
    vec3 normal_worldCoord = (m*vec4(normal,0)).xyz; // takes into account rotation of the model
    N = normalize(normal_worldCoord);
    V = normalize(cameraPosition-position_worldCoord); //unit vector vertex -> camera
}

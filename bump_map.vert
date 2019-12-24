#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;
layout(location=3) in vec3 tangent;

out vec2 uv;
out vec3 n;
out vec3 l;
out vec3 v;
out vec3 T;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

void main() {
    gl_Position = mvp * vec4(position, 1);
    uv = vertexUV;
    
    T = normalize((m * vec4(tangent,   0.0)).xyz);
    
    // Vectors n, l, v for lighting
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    l = normalize(lightPosition-position_worldCoord); // unit vector vertex -> light
    vec3 normal_worldCoord = (m*vec4(normal,0)).xyz; // takes into account rotation of the model
    n = normalize(normal_worldCoord);
    v = normalize(cameraPosition-position_worldCoord); //unit vector vertex -> camera
    
}

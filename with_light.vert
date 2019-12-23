#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;

out vec2 uv;
out vec3 n;
out vec3 l;
out vec3 v;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

void main() {
    gl_Position = mvp * vec4(position, 1);
    uv = vec2(vertexUV.x, vertexUV.y);
    
    // Vectors n, l, v for lighting
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    l = normalize(lightPosition-position_worldCoord); // unit vector vertex -> light
    n = normalize(normal);
    v = normalize(cameraPosition-position_worldCoord); //unit vector vertex -> camera
    
    
//    vec3 vertexPosition_cameraspace = ( v * m * vec4(position,1)).xyz;      //vertex position to camera space
//    vec3 EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;    //vertex->eye direction
//
//    vec3 LightPosition_cameraspace = ( v * vec4(lightPosition,1)).xyz;      //light position to camera space
//    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace; //vertex->light direction

    
//    Normal_cameraspace = ( v * m * vec4(normal,0)).xyz;
}

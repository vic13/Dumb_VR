#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;

out VS_OUT {
    vec2 uv;
    vec3 L_pointlight;
    vec3 L_flashlight;
    vec3 flashlightDirection;
    vec3 N;
    vec3 V;
} vs_out;

uniform vec3 pointlightPosition;
uniform vec3 flashlightPosition;
uniform vec3 flashlightDirection;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

void main() {
    gl_Position = mvp * vec4(position, 1);
    vs_out.uv = vertexUV;
    
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    
    // The following vectors are normalized in the fragment shader (useless to normalize before interpolation)
    vs_out.L_pointlight = pointlightPosition-position_worldCoord;  // vector vertex -> point light
    vs_out.L_flashlight = flashlightPosition-position_worldCoord;  // vector vertex -> flashlight
    vs_out.flashlightDirection = flashlightDirection;
    vs_out.N = (m*vec4(normal,0)).xyz;;                            // takes into account rotation of the model
    vs_out.V = cameraPosition-position_worldCoord;                 // vector vertex -> camera
    
}

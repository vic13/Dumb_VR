#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

struct PointLight {
    vec3 position;
};
#define NR_POINT_LIGHTS 10

uniform vec3 L_directional;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 flashlightPosition;
uniform vec3 flashlightDirection;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

out VS_OUT {
    vec2 uv;
    vec3 L_pointlights[NR_POINT_LIGHTS];
    vec3 L_flashlight;
    vec3 L_directional;
    vec3 flashlightDirection;
    vec3 N;
    vec3 V;
} vs_out;

void main() {
    gl_Position = mvp * vec4(position, 1);
    vs_out.uv = vertexUV;
    
    vec3 position_worldCoord = (m*vec4(position,1)).xyz;
    
    // TBN
    vec3 normal_worldCoord = (m*vec4(normal, 0)).xyz;;                            // takes into account rotation of the model
    vec3 N = normalize(normal_worldCoord);
    vec3 tangent_worldCoord = (m * vec4(tangent, 0)).xyz;
    vec3 T = normalize(tangent_worldCoord);
    vec3 bitangent_worldCoord = (m * vec4(bitangent, 0)).xyz;
    vec3 B = normalize(bitangent_worldCoord);
    mat3 TBN = mat3(T, B, N);
    mat3 invTBN = transpose(TBN); // cause orthogonal
    // L for directional light
    vs_out.L_directional = invTBN * L_directional;                  // tangent space
    // L for point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        vec3 L_pointlight = pointLights[i].position-position_worldCoord;  // vector vertex -> point light
        vs_out.L_pointlights[i] = invTBN * L_pointlight;                  // tangent space
    }
    // L for flashlight
    vec3 L_flashlight = flashlightPosition-position_worldCoord;     // vector vertex -> flashlight
    vs_out.L_flashlight = invTBN * L_flashlight;                    // tangent space
    // Flashlight direction
    vs_out.flashlightDirection = invTBN * flashlightDirection;      // tangent space
    
    // V
    vec3 V = cameraPosition-position_worldCoord;                    // vector vertex -> camera
    vs_out.V = invTBN * V;                                          // tangent space
    
    // N not used
    vs_out.N = vec3(0.0);
}

#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

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
    vec3 L_pointlight = pointlightPosition-position_worldCoord;  // vector vertex -> point light
    vec3 L_flashlight = flashlightPosition-position_worldCoord;  // vector vertex -> flashlight
    vec3 N = (m*vec4(normal,0)).xyz;;                            // takes into account rotation of the model
    vec3 V = cameraPosition-position_worldCoord;                 // vector vertex -> camera
    
    
    N = normalize(N);
    vec3 tangent_worldCoord = (m * vec4(tangent, 0)).xyz;
    vec3 T = normalize(tangent_worldCoord);
    vec3 bitangent_worldCoord = (m * vec4(bitangent, 0)).xyz;
    vec3 B = normalize(bitangent_worldCoord);
    mat3 TBN = mat3(T, B, N);
    mat3 invTBN = transpose(TBN); // cause orthogonal
    
    vec3 L_pointlight_tangent = invTBN * L_pointlight;
    vec3 L_flashlight_tangent = invTBN * L_flashlight;
    vec3 flashlightDirection_tangent = invTBN * flashlightDirection;
    vec3 V_tangent = invTBN * V;
    
    vs_out.L_pointlight = L_pointlight_tangent;
    vs_out.L_flashlight = L_flashlight_tangent;
    vs_out.flashlightDirection = flashlightDirection_tangent;
    vs_out.N = vec3(0.0); //not used
    vs_out.V = V_tangent;
    
}

#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 vertexUV;
layout(location=3) in vec3 tangent;
layout(location=4) in vec3 bitangent;

out vec2 uv;
out vec3 L_tangent;
out vec3 V_tangent;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

void main() {
    gl_Position = mvp * vec4(position, 1);
    
    vec3 position_worldCoord = (m*vec4(position, 1)).xyz;
    
    vec3 normal_worldCoord = (m*vec4(normal, 0)).xyz; // takes into account rotation of the model
    vec3 N = normalize(normal_worldCoord);
    vec3 tangent_worldCoord = (m * vec4(tangent, 0)).xyz;
    vec3 T = normalize(tangent_worldCoord);
    vec3 bitangent_worldCoord = (m * vec4(bitangent, 0)).xyz;
    vec3 B = normalize(bitangent_worldCoord);
    mat3 TBN = mat3(T, B, N);
    mat3 invTBN = transpose(TBN); // cause orthogonal
    
    uv = vertexUV;
    vec3 l = normalize(lightPosition-position_worldCoord); // unit vector vertex -> light
    vec3 v = normalize(cameraPosition-position_worldCoord); //unit vector vertex -> camera
    L_tangent = invTBN * l;
    V_tangent = invTBN * v;
    
}

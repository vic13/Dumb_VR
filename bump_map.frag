#version 330 core

in vec3 n;
in vec3 l;
in vec3 v;
in vec3 T;

in vec2 uv;

uniform sampler2D texSampler;
uniform sampler2D normalTexSampler;

out vec4 color;

void main() {
    
    //color = vec4(T, 1.0);
    vec4 color0 = texture(texSampler, uv);
    color = texture(normalTexSampler, uv);
}

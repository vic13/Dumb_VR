#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform vec3 lightColor;

void main() {
    vec3 color0 = texture(texture_diffuse1, uv).xyz;
    
    // Only ambient
    float ambient_strength = 1;
    vec3 ambient = ambient_strength * lightColor;
    
    color = vec4(color0 * ambient, 1);
}

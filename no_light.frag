#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D texSampler;
uniform vec3 lightColor;

void main() {
    vec3 color0 = texture(texSampler, uv).xyz;
    color = vec4(color0.x*lightColor.x, color0.y*lightColor.y, color0.z*lightColor.z, 1.0);
}

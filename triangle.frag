#version 330 core

in vec3 myColor;
out vec4 color;

uniform float intensity;

void main(){
    color = vec4(myColor*intensity, 1.0);
}

#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
out vec3 myColor;

uniform float scale;
uniform mat4 mvp;

void main(){
    //gl_Position = vec4(position*scale, 1);
    gl_Position = mvp * vec4(position, 1);
    myColor = color;
}

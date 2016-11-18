#version 330 core

layout (location = 0)	in vec3 in_position;
layout (location = 4)	in mat4 in_instanceMat;

out vec4 v2f_position;

uniform mat4 modelMat;
uniform mat4 lightMat;

void main() {
    gl_Position = lightMat * modelMat * in_instanceMat * vec4(in_position, 1.0);
    v2f_position = gl_Position;
}
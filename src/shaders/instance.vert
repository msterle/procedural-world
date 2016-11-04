#version 330 core
  
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;
layout (location = 4) in mat4 in_instanceMat;

out vec4 v2f_positionC; // Position in camera space.
out vec4 v2f_normalC;   // normal in camera space.
out vec4 v2f_color;

uniform mat4 modelMat
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
    v2f_positionC = viewMat * modelMat * in_instanceMat * vec4(in_position, 1);
    v2f_normalC = viewMat * modelMat * in_instanceMat * vec4(in_normal, 0);
    v2f_color = vec4(in_color, 1);
    gl_Position = projMat * viewMat * modelMat * in_instanceMat * vec4(in_position, 1.0);
}
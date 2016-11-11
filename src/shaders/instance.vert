#version 330 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

const int MY_ARRAY_SIZE = 512;

layout (std140) uniform modelMatsBlock {
	mat4 modelMats [MY_ARRAY_SIZE];
};

layout (location = 0)	in vec3 in_position;
layout (location = 1)	in vec3 in_normal;
layout (location = 2)	in vec3 in_color;
layout (location = 4)	in mat4 in_instanceMat;
layout (location = 8)	in vec4 in_materialAmbient;
layout (location = 9)	in vec4 in_materialDiffuse;
layout (location = 10)	in vec4 in_materialSpecular;
layout (location = 11)	in float in_materialShininess;

out vec4 v2f_positionC; // Position in camera space.
out vec4 v2f_normalC;   // normal in camera space.
out vec4 v2f_color;
out Material v2f_material;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main() {
    v2f_positionC = viewMat * modelMat * in_instanceMat * vec4(in_position, 1);
    v2f_normalC = viewMat * modelMat * in_instanceMat * vec4(in_normal, 0);
    v2f_color = vec4(in_color, 1);
    v2f_material.ambient = in_materialAmbient;
    v2f_material.diffuse = in_materialDiffuse;
    v2f_material.specular = in_materialSpecular;
    v2f_material.shininess = in_materialShininess;
    gl_Position = projMat * viewMat * modelMat * in_instanceMat * vec4(in_position, 1.0);
}
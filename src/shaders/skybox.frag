#version 330 core

in vec3 v2f_texcoords;
out vec4 color;

uniform samplerCube skyboxTex;

void main() {
	color = texture(skyboxTex, v2f_texcoords);
    //color = vec4(v2f_texcoords, 1);
}
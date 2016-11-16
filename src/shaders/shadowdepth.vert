#version 330 core

layout (location = 0)	in vec3 in_position;
layout (location = 3)	in vec2 in_texCoord;

out vec2 TexCoords;

void main() {
	TexCoords = in_texCoord;
    gl_Position = vec4(in_position, 1.0);
}
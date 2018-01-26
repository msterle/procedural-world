#version 330 core

in vec2 v2f_texCoords;
uniform sampler2D inTex1;
uniform sampler2D inTex2;

out vec4 fragColor;

void main() {
	fragColor = texture(inTex1, v2f_texCoords) * texture(inTex2, v2f_texCoords);
} 
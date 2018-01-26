#version 330 core

in vec2 v2f_texCoords;
uniform sampler2D inTex1;
uniform vec4 colorDark;
uniform vec4 colorLight;

out vec4 fragColor;

void main() {
	fragColor = mix(colorDark, colorLight, texture(inTex1, v2f_texCoords));
} 
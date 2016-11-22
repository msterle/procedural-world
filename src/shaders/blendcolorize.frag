#version 330 core

in vec2 v2f_texCoords;
uniform sampler2D inTex1;
uniform vec4 colorDark;
uniform vec4 colorLight;

void main() {
	gl_FragColor = mix(colorDark, colorLight, texture2D(inTex1, v2f_texCoords));
} 
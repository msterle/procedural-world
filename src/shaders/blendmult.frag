#version 330 core

in vec2 v2f_texCoords;
uniform sampler2D inTex1;
uniform sampler2D inTex2;

void main() {
	gl_FragColor = texture2D(inTex1, v2f_texCoords) * texture2D(inTex2, v2f_texCoords);
} 
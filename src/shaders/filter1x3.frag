#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offsets[3];
uniform float kernel[3];

void main() {
	vec4 color = vec4(0);
	color += texture2D(tex, v2f_texCoords + offsets[0]) * kernel[0];
	color += texture2D(tex, v2f_texCoords + offsets[1]) * kernel[1];
	color += texture2D(tex, v2f_texCoords + offsets[2]) * kernel[2];
	gl_FragColor = color;
} 
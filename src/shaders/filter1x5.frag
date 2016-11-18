#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offsets[9];
uniform float kernel[9];

void main() {
	vec4 color = vec4(0);
	color += texture2D(tex, v2f_texCoords + offsets[0]) * kernel[0];
	color += texture2D(tex, v2f_texCoords + offsets[1]) * kernel[1];
	color += texture2D(tex, v2f_texCoords + offsets[2]) * kernel[2];
	color += texture2D(tex, v2f_texCoords + offsets[3]) * kernel[3];
	color += texture2D(tex, v2f_texCoords + offsets[4]) * kernel[4];
	gl_FragColor = color;
} 
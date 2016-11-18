#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offset[9];
uniform float kernel[9];

void main() {
	vec4 color = vec4(0);
	color += texture2D(tex, v2f_texCoords + offset[0]) * kernel[0];
	color += texture2D(tex, v2f_texCoords + offset[1]) * kernel[1];
	color += texture2D(tex, v2f_texCoords + offset[2]) * kernel[2];
	color += texture2D(tex, v2f_texCoords + offset[3]) * kernel[3];
	color += texture2D(tex, v2f_texCoords + offset[4]) * kernel[4];
	color += texture2D(tex, v2f_texCoords + offset[5]) * kernel[5];
	color += texture2D(tex, v2f_texCoords + offset[6]) * kernel[6];
	color += texture2D(tex, v2f_texCoords + offset[7]) * kernel[7];
	color += texture2D(tex, v2f_texCoords + offset[8]) * kernel[8];
	gl_FragColor = color;
} 
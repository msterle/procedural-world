#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offsets[5];
uniform float kernel[5];

out vec4 fragColor;

void main() {
	vec4 color = vec4(0);
	color += texture(tex, v2f_texCoords + offsets[0]) * kernel[0];
	color += texture(tex, v2f_texCoords + offsets[1]) * kernel[1];
	color += texture(tex, v2f_texCoords + offsets[2]) * kernel[2];
	color += texture(tex, v2f_texCoords + offsets[3]) * kernel[3];
	color += texture(tex, v2f_texCoords + offsets[4]) * kernel[4];
	fragColor = color;
} 
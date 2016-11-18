#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offsets[9];
uniform float kernel[9];
uniform vec2 scale;

void main() {
	vec4 color = vec4(0);
	color += texture2D(tex, v2f_texCoords + offsets[0] / scale) * kernel[0];
	color += texture2D(tex, v2f_texCoords + offsets[1] / scale) * kernel[1];
	color += texture2D(tex, v2f_texCoords + offsets[2] / scale) * kernel[2];
	color += texture2D(tex, v2f_texCoords + offsets[3] / scale) * kernel[3];
	color += texture2D(tex, v2f_texCoords + offsets[4] / scale) * kernel[4];
	color += texture2D(tex, v2f_texCoords + offsets[5] / scale) * kernel[5];
	color += texture2D(tex, v2f_texCoords + offsets[6] / scale) * kernel[6];
	color += texture2D(tex, v2f_texCoords + offsets[7] / scale) * kernel[7];
	color += texture2D(tex, v2f_texCoords + offsets[8] / scale) * kernel[8];
	gl_FragColor = texture2D(tex, v2f_texCoords + offsets[8] / scale);
} 
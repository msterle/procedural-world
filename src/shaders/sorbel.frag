#version 330 core

in vec2 v2f_texCoords;

uniform sampler2D tex;
uniform vec2 offsets[9];

mat2x3 sx = mat2x3(
	-1.0, 1.0,
	-2.0, 2.0,
	-1.0, 1.0);

mat3x2 sy = mat3x2(
	 1.0,  2.0,  1.0,
	-1.0, -2.0, -1.0);

void main() {
	vec4 gx = vec4(0), gy = vec4(0);
	
	gx += texture2D(tex, v2f_texCoords + offsets[0]) * sx[0][0];
	gx += texture2D(tex, v2f_texCoords + offsets[2]) * sx[1][0];
	gx += texture2D(tex, v2f_texCoords + offsets[3]) * sx[0][1];
	gx += texture2D(tex, v2f_texCoords + offsets[5]) * sx[1][1];
	gx += texture2D(tex, v2f_texCoords + offsets[6]) * sx[0][2];
	gx += texture2D(tex, v2f_texCoords + offsets[8]) * sx[1][2];

	gy += texture2D(tex, v2f_texCoords + offsets[0]) * sy[0][0];
	gy += texture2D(tex, v2f_texCoords + offsets[1]) * sy[1][0];
	gy += texture2D(tex, v2f_texCoords + offsets[2]) * sy[2][0];
	gy += texture2D(tex, v2f_texCoords + offsets[6]) * sy[0][1];
	gy += texture2D(tex, v2f_texCoords + offsets[7]) * sy[1][1];
	gy += texture2D(tex, v2f_texCoords + offsets[8]) * sy[2][1];

	gl_FragColor = 1.0 - sqrt(pow(gx, vec4(2)) + pow(gy, vec4(2)));
} 
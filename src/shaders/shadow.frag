#version 330 core

in vec4 v2f_position;

void main() {
	float depth = v2f_position.z / v2f_position.w ;
	depth = depth * 0.5 + 0.5;

	float moment1 = depth;
	float moment2 = depth * depth;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25*(dx*dx+dy*dy);
	

	gl_FragColor = vec4(moment1, moment2, 0.0, 0.0);
} 
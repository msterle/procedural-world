#version 330 core
  
//layout (location = 0) 
in vec3 position;
in vec3 in_colour;

out vec3 Colour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	Colour = in_colour;
    gl_Position = proj * view * model * vec4(position, 1.0f);
}
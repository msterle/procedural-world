#version 330 core

in vec3 Colour;
out vec4 out_colour;

void main()
{
    out_colour = vec4(Colour, 1.0f);
} 
#version 330 core

in vec4 fragmentPosition;
in vec4 frag_color;
out vec4 output_color;

void main()
{
    float gamma = 2.2;



	output_color = pow(frag_color, vec4(1.0/2.2));

	
}
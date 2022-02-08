#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;

out vec4 frag_color;

void main()
{
	/* Select any 2 color as corner colors
		Use simple linear function to change it with respect to y
		since we I have background not centered at y=0 axis, I have to make some adjustment so that 3.6 acts as the center
	*/
    gl_Position = projectionMatrix * inverse(cameraTransform) * worldTransform * pos;
	vec3 top_color = vec3(98.0f, 145.0f, 247.0f);
	vec3 bot_color = vec3(22.0f, 53.0f, 119.0f);
	vec3 mid_color = top_color + bot_color / 2.0f;
	vec3 perVColor = (mid_color - ((pos[1] - 3.6f) / 5.1f) * (mid_color - bot_color)) / 255.0f;
    frag_color = vec4(perVColor[0], perVColor[1], perVColor[2], 1.0f);
}
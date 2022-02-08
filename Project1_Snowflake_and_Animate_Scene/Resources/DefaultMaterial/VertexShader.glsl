#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;
uniform vec4 color;

out vec4 frag_color;

void main()
{
    gl_Position = projectionMatrix * inverse(cameraTransform) * worldTransform * pos;
    frag_color = color;
}
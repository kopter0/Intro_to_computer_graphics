#version 330 core

layout(location = 0) in vec4 pos;

uniform mat4 worldTransform;
uniform mat4 LightProj;

void main(){
	gl_Position = LightProj * worldTransform * pos;
}

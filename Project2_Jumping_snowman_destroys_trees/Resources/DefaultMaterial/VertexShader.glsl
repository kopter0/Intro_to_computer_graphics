#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 normal;


uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;
uniform vec4 color;

out vec4 fragmentPosition;
out vec4 frag_color;
out vec4 fragmentNormal;

mat4 NormalMatrix(mat4 MVM)
{
	mat4 invm = inverse(MVM);
	invm[0][3] = 0;
	invm[1][3] = 0;
	invm[2][3] = 0;

	return transpose(invm);
}


void main()
{
	// Output position of the vertex, in clip space : MVP * position
	mat4 MVM = inverse(cameraTransform) * worldTransform;
	mat4 NVM = NormalMatrix(MVM);

	vec4 wPosition = MVM * vec4(pos);
	fragmentPosition = wPosition;
	fragmentNormal = NVM * normal;
	gl_Position = projectionMatrix * wPosition;
	

//    gl_Position = projectionMatrix * inverse(cameraTransform) * worldTransform * pos;
    frag_color = color;
}
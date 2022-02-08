#version 330 core

layout(location = 0) in vec4 pos;



uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;
uniform vec4 color;

out vec4 fragmentPosition;
out vec4 frag_color;




void main()
{
	// Output position of the vertex, in clip space : MVP * position
	mat4 MVM = inverse(cameraTransform) * worldTransform;


	vec4 wPosition = MVM * vec4(pos);
	fragmentPosition = wPosition;

	gl_Position = projectionMatrix * wPosition;
	

    frag_color = color;
}
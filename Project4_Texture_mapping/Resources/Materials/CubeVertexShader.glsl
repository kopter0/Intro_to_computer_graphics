#version 330 core

layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv_in;
layout(location = 3) in vec4 tangent;

struct Light
{
	vec3 pos;
};

uniform int numLights;
uniform Light lights[3];

uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;
uniform vec4 color;
uniform bool is_skybox;

out vec4 fragmentPosition;
out vec4 fragmentNormal;
out vec3 Texcoords;
out vec3 TNormal;
out vec3 BNormal;
out vec3 LPos[3];
out vec2 uv;
out vec3 V;
out mat3 TBN;
out vec3 TangentFragPos;


mat4 NormalMatrix(mat4 MVM)
{
	mat4 invm = inverse(MVM);
	invm[3][0] = 0;
	invm[3][1] = 0;
	invm[3][2] = 0;

	return transpose(invm);
}


void main()
{
	// Make WorldSpace (but Model Space is same as World Space now)
	Texcoords = vec3(worldTransform*pos);
	uv = uv_in;
	mat4 camT;
	if(is_skybox)
	{
		camT = mat4(mat3(cameraTransform));	
		camT[3][3] = 1;
	}
	else
	{
		camT = cameraTransform;
	}
	// Output position of the vertex, in clip space : MVP * position
	mat4 MVM = inverse(camT) * worldTransform;
	mat4 NVM = NormalMatrix(MVM);
	
	vec4 wPosition = MVM * vec4(pos);
	fragmentPosition = wPosition;
	fragmentNormal = NVM * normal;
	
	vec3 TNormal = normalize((NVM * tangent).xyz);
	TNormal = normalize(TNormal - dot(TNormal, fragmentNormal.xyz) * fragmentNormal.xyz);
	vec3 BNormal = cross(fragmentNormal.xyz, TNormal);
	TBN = transpose(mat3(TNormal, BNormal, fragmentNormal.xyz));
	TangentFragPos = TBN * fragmentPosition.xyz;
	vec3 L;
	for (int i = 0; i < numLights; i++){
		LPos[i] = vec3(MVM * vec4(lights[i].pos, 1));
		LPos[i] = lights[i].pos;
		L.x = dot(TNormal, LPos[i] - wPosition.xyz);
		L.y = dot(BNormal, LPos[i] - wPosition.xyz);
		L.z = dot(fragmentNormal.xyz, LPos[i] - wPosition.xyz);
		LPos[i] = normalize(L);
		LPos[i] = TBN * lights[i].pos;
	}

	V.x = dot(TNormal, -wPosition.xyz);
	V.y = dot(BNormal, -wPosition.xyz);
	V.z = dot(fragmentNormal.xyz, -wPosition.xyz);
	V = normalize(V);

	gl_Position = projectionMatrix * wPosition;
}


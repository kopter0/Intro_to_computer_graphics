#version 330 core


layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 uv_in;
layout (location = 3) in vec4 tangent;

struct Light
{
	int type;
	int enabled;
	float exp;
	float angle;
	vec3 dir;
	vec3 pos;
	vec3 illu_amb;
	vec3 illu_diff;
	vec3 illu_spec;
};

uniform int numLights;
uniform Light lights[5];

uniform mat4 worldTransform;
uniform mat4 cameraTransform;
uniform mat4 projectionMatrix;

out vec2 uv;
out vec3 tanFragPos;
out vec3 tanFragNorm;
out vec3 Texcoords;
out vec3 tanToLight[5];
out vec3 tanLightPosition[5];
out mat3 TBN;

mat4 NormalMatrix(mat4 MVM)
{
	mat4 invm = inverse(MVM);
	invm[3][0] = 0;
	invm[3][1] = 0;
	invm[3][2] = 0;

	return transpose(invm);
}

void main(){
	mat4 WNM = NormalMatrix(worldTransform);
	uv = uv_in;
	
	vec4 wPosition = worldTransform * pos;
	vec4 wNormal = WNM * normal;

	// compute TBN matrix
	vec3 N = normalize(vec3(wNormal));
	vec3 T = normalize(vec3(worldTransform * tangent));
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	
	TBN = transpose(mat3(T, B, N));

	tanFragPos = TBN * vec3(wPosition);
	tanFragNorm = TBN * vec3(wNormal);

	for (int i = 0; i < numLights; i++){
		tanToLight[i] = TBN * (lights[i].pos - vec3(wPosition));
		tanLightPosition[i] = TBN * (lights[i].pos);
	}

	gl_Position = projectionMatrix * inverse(cameraTransform) * wPosition;

}
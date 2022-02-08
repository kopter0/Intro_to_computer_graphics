#version 330 core

in vec4 fragmentPosition;
in vec4 fragmentNormal;
in vec3 Texcoords;
in vec3 TNormal;
in vec3 BNormal;
in vec3 LPos[3];
in vec2 uv;
in vec3 V;
in mat3 TBN;
in vec3 TangentFragPos;
in vec4 LightFragPos;

uniform int numLights;
uniform samplerCube skybox;
out vec4 output_color;

uniform sampler2D DepthMap;
uniform sampler2D normal_map;	
uniform mat4 cameraTransform;
uniform bool is_skybox;

void main()
{
	output_color = vec4(0);
	vec3 L;
	vec3 V;
	if(is_skybox)
	{
		output_color = texture(skybox, Texcoords);	
	}
	else
	{
		vec3 V = normalize(vec3(fragmentPosition));
		vec3 N = normalize(vec3(fragmentNormal));
		vec3 R = reflect(V, N);
		R = vec3(cameraTransform*vec4(R, 0.0));

		vec4 total = vec4(0);

		for (int i = 0; i < numLights; i++){
			vec4 Tex = texture(normal_map, uv);
			vec3 DN = normalize(2.0 * Tex.xyz - 1);
			vec3 TL = normalize(LPos[i] - TangentFragPos);
			float kd = max(dot(DN, TL), 0.0f);
			total += kd * texture(skybox, Texcoords);
		}
		
		output_color += total;
	}
	
}

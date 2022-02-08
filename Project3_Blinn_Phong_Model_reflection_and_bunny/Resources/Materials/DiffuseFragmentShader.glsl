#version 330 core

in vec4 fragPos;
in vec4 fragNorm;
out vec4 output_color;


struct Light
{
	int type;
	int enabled;
	vec3 lookAt;
	float angle;
	float exponent;
	vec3 diffuse_illuminance;
	vec3 pos;
};

uniform mat4 cameraTransform;
uniform int numLights;
uniform Light lights[3];

uniform vec3 diffuse_reflectance;

void main()
{
	vec3 intensity = vec3(0.f);
	vec3 normal = normalize(fragNorm.xyz);

	mat4 worldToCamera = inverse(cameraTransform);
	for (int i=0; i<numLights; i++)
	{
		vec4 pos;
		vec3 tolight_Unnorm;
		vec3 tolight;
		
		float attenuation = 1;
		float spotComp = 1;

		switch(lights[i].type){
			case 1:
				pos = worldToCamera * vec4(lights[i].pos, 1);
				tolight = normalize(pos.xyz - fragPos.xyz);
				float dist = sqrt(length(pos - fragPos));
				attenuation = pow((dist / 2.f + 1), 2);
				break;
			case 2:
				pos = worldToCamera * vec4(lights[i].pos, 0);
				tolight = normalize(pos.xyz - fragPos.xyz);
				break;
			case 3:
				pos = worldToCamera * vec4(lights[i].pos, 1);
				tolight_Unnorm = pos.xyz - fragPos.xyz;
				tolight = normalize(pos.xyz - fragPos.xyz);
				vec3 dir = normalize((worldToCamera * vec4(lights[i].lookAt, 1.f) - pos).xyz);
				float phi = acos(dot(dir, -tolight));
				spotComp = 0;
				if (phi < lights[i].angle)
					spotComp = pow(cos(phi), lights[i].exponent);
				break;
		}

		float diffuse = max(0, dot(normal, tolight));

		intensity += diffuse_reflectance * lights[i].diffuse_illuminance * diffuse * spotComp / attenuation;
	}

	output_color = vec4(intensity, 1);
	output_color.rgb = pow(output_color.rgb, vec3(1.0/2.2));

	
}

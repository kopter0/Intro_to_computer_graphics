#version 330 core

in vec4 fragPos;
in vec4 fragNorm;

out vec4 output_color;

struct Light{
	int type;
	int enabled;
	vec3 illu_amb;
	vec3 illu_dif;
	vec3 illu_spec;
	vec3 pos;
	vec3 lookAt;
	float angle;
	float exponent;
	float size;
};

uniform mat4 cameraTransform;
uniform int numLights;
uniform Light lights[3];

uniform float shininess;
uniform vec3 ambientRef;
uniform vec3 diffuseRef;
uniform vec3 specularRef;

void main(){
	vec3 total = vec3(0);
	vec3 N = normalize(fragNorm.xyz);

	mat4 worldToCamera = inverse(cameraTransform);

	//Same as phong, except intensity

	for (int i = 0; i < numLights; i++){
		if (lights[i].enabled == 0)
			continue;
		vec3 P = (worldToCamera * vec4(lights[i].pos, 1)).xyz;
		vec3 L = normalize(P - fragPos.xyz);
		if (lights[i].type == 2)
			L = normalize(P);
		vec3 E = normalize(-fragPos.xyz);

		float attenuation = 1.0f;
		float spot_in = 1.0f;
		float Kd = dot(L, N);

		// several levesl of intensity, depending how light falls

		vec3 intensity = vec3(0);
		if (Kd > 0.95)
			intensity = vec3(1);
		else if (Kd > 0.6)
			intensity = vec3(0.6);
		else if (Kd > 0.3)
			intensity = vec3(0.3);
		else if (Kd > 0)
			intensity = vec3(0.15);
		else
			intensity = vec3(0.05);
		
		switch(lights[i].type){
			case 1:
				float dist = length(P + E);
				attenuation = pow(dist / lights[i].size + 1, 2);
				break;
			case -1:
				dist = length(P + E);
				attenuation = (dist / lights[i].size + 1);
				break;
			case 2:
				break;
			case 3:
				vec3 dir = normalize((worldToCamera * vec4(lights[i].lookAt, 1)).xyz - P);
				float delta = acos(dot(dir, -L));
				if (delta > lights[i].angle){
					spot_in = 0.f;
					break;
				}
				spot_in = pow(dot(dir, L), lights[i].exponent);
				break;
		}

		vec3 aComp = ambientRef * lights[i].illu_amb;
		vec3 dComp = diffuseRef * lights[i].illu_dif;

		vec3 H = normalize(E + L);
		vec3 sComp = specularRef * lights[i].illu_spec * pow(max(dot(H, N), 0.f), shininess);

		// multiply by intensity to get different levels

		total += intensity * (aComp + (dComp + sComp) * spot_in / attenuation);
		
	}


	output_color = vec4(total, 1);
	output_color.rgb = pow(output_color.rgb, vec3(1.0/2.2));
}
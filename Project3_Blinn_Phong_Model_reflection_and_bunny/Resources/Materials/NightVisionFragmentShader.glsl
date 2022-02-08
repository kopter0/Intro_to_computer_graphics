#version 330 core

in vec4 fragPos;
in vec4 fragNorm;

out vec4 output_color;

struct Light{
	int type;
	vec3 pos;
	int enabled;
	vec3 illu_amb;
	vec3 illu_dif;
	vec3 illu_spec;
	vec3 lookAt;
	float exponent;
	float angle;
};

uniform mat4 cameraTransform;
uniform int numLights;
uniform Light lights[3];

uniform float shininess;
//uniform float 
uniform float diffuseRef;

/* 
	Only green component is relevant here, so every computation must contribute only to it
*/

void main(){
	vec3 N = normalize(fragNorm.xyz);

	mat4 worldToCamera = inverse(cameraTransform);

	vec3 E = normalize(-fragPos.xyz);
	
	//regardless of lightning we have at least some visibility
	float total = max(dot(E, N), 0) / 2;


	// compute as in phong
	for (int i = 0; i < numLights; i++){
		if (lights[i].enabled == 0)
			continue;
		vec3 P = (worldToCamera * vec4(lights[i].pos, 1)).xyz;
		vec3 L = normalize(P - fragPos.xyz);
		vec3 H = normalize(L + E);

		float intensity = max(dot(L, N), 0);

		vec3 sComp = lights[i].illu_spec * pow(max(dot(N, H), 0), shininess);

		float spotFac = 0;
		if (lights[i].type == 3){
			vec3 dir = normalize((worldToCamera * vec4(lights[i].lookAt, 1)).xyz - P);
			float delta = acos(dot(dir, -L));
			if (delta > lights[i].angle){
				spotFac = 0.f;
			}
			else
				spotFac = pow(dot(E, N), lights[i].exponent);
		}

		total += sComp.y + spotFac;
	}



	output_color = vec4(0, total, 0, 1);
	output_color.rgb = pow(output_color.rgb, vec3(1.0/2.2));
}

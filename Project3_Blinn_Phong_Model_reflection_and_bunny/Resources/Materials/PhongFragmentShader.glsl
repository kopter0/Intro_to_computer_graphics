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
uniform vec3 fogC;

void main(){
	vec3 total = vec3(0);
	vec3 N = normalize(fragNorm.xyz);

	mat4 worldToCamera = inverse(cameraTransform);
	
	for (int i = 0; i < numLights; i++){
		if (lights[i].enabled == 0)
			continue;
		// compute basic vectors
		vec3 P = (worldToCamera * vec4(lights[i].pos, 1)).xyz;
		vec3 L = normalize(P - fragPos.xyz);
		// if directional light use postion as direction
		if (lights[i].type == 2)
			L = normalize(P);	
		vec3 E = normalize(-fragPos.xyz);
		vec3 H = normalize(L + E);

		// ambient part does not depend on anything
		vec3 aComp = ambientRef * lights[i].illu_amb;

		// depends on relative position of light and normal
		vec3 dComp = diffuseRef * max(dot(L, N), 0) * lights[i].illu_dif;
		
		// depend on angle made buy normal and halfAngle
		vec3 sComp = specularRef * lights[i].illu_spec * pow(max(dot(N, H), 0), shininess);


		float attenuation = 1.f;
		float spot_in = 1.0f;
		
		// compute attenuation and whether object in cone of spotlight
		switch (lights[i].type) {
			case -1:
				// Point light linear degradation
				
				float dist = length(P - fragPos.xyz);
				attenuation = dist / lights[i].size + 1;
				break;
			case 1:
				// Point light normal degradation
				
				dist = length(P - fragPos.xyz);
				attenuation = pow((dist / lights[i].size) + 1, 2);
				break;
			case 2:
				break;
			case 3:
				dist = length(P - fragPos.xyz);
				
				// I use again linear degradation for sake of visiblilty, can be change to squared to be physically correct
				
				attenuation = pow(dist / lights[i].size + 1, 1);
				vec3 dir = normalize((worldToCamera * vec4(lights[i].lookAt, 1)).xyz - P);
				float delta = acos(dot(dir, -L));
				// if out of cone set to 0
				if (delta > lights[i].angle){
					spot_in = 0.f;
					break;
				}
				// if in, assign intensity
				spot_in = pow(dot(dir, L), lights[i].exponent);
				break;
			default:
				break;
		}

		// the final color, before adding fog
		vec3 interm = aComp + (dComp + sComp) * spot_in / attenuation;

		total += interm;
	}

	//make fog at a distance

	float distFac = length(fragPos.xyz);
	float fogFac = (15.f - distFac)/(8.f);
	fogFac = clamp(fogFac, 0, 1);
	total = mix(fogC, total, fogFac);

	output_color = vec4(total, 1);
	output_color.rgb = pow(output_color.rgb, vec3(1.0/2.2));
}
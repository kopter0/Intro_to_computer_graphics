#version 330 core


struct Light {
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


in vec2 uv;
in vec3 tanFragPos;
in vec3 tanFragNorm;
in vec3 tanLightPosition[5];
in vec3 tanToLight[5];
in mat3 TBN;


uniform int numLights;
uniform Light lights[5];
uniform vec3 ambRef;
uniform vec3 diffRef;
uniform vec3 specRef;
uniform int shininess;
uniform float transparency;
uniform mat4 cameraTransform;

uniform sampler2D normalMap;

out vec4 output_color;

void main(){
	vec3 total = vec3(0);

	vec3 camPos = TBN * vec3(cameraTransform * vec4(0,0,0,1));

	for (int i = 0; i < numLights; i++){
		if (lights[i].enabled == 0)
			continue;

		float kd, ks;

		vec3 N = texture(normalMap, uv).xyz;
		// make range (-1, 1), and use it as normal of fragment
		N = normalize(2 * N - 1);

		
		vec3 toLight = tanToLight[i];

		if (lights[i].type == 2)
			toLight = tanLightPosition[i];

		toLight = normalize(toLight);
		kd = max(dot(N, toLight), 0.f);

		vec3 ref = reflect(-toLight, N);
			   
		ks = pow(max(dot(N, ref), 0), shininess);


		float attenuation = 1.f;
		float spot_in = 1.0f;
		float dist = length(tanLightPosition[i] - tanFragPos);
		if (lights[i].type == 1)
			attenuation = pow(dist / 2 + 1, 2);
		else if (lights[i].type == 3){
			vec3 dir = normalize(TBN * lights[i].dir - tanLightPosition[i]);
			float delta = acos(dot(dir, -toLight));
			spot_in = pow(dot(dir, -toLight), 64.f);
			if (delta > lights[i].angle)
				spot_in = 0;
		}


		vec3 aComp = ambRef * lights[i].illu_amb;
		vec3 dComp = kd * diffRef * lights[i].illu_diff;
		vec3 sComp = ks * specRef * lights[i].illu_spec;

		total += (aComp + dComp + sComp) * spot_in / attenuation;

	}
	output_color = vec4(total, transparency);
}
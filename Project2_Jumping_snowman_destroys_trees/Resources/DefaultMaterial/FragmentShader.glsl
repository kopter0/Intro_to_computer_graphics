#version 330 core

in vec4 fragmentPosition;
in vec4 frag_color;
in vec4 fragmentNormal;
out vec4 output_color;

void main()
{
    float gamma = 2.2;

	vec3 tolight = normalize(vec3(10.0f) - fragmentPosition.xyz);
	vec3 normal = normalize(fragmentNormal.xyz);

	float diffuse = max(0.0, dot(normal, tolight));
	vec4 intensity = frag_color * diffuse;

	output_color = pow(intensity, vec4(1.0/2.2));

	
}
#pragma once

#include <glm/glm.hpp>

#include <Transform.hpp>

#define MAX_LIGHTS 3
// Feel free to increase maximum number of lights!

enum LightType : int
{
	PointLightLinear = -1,
	PointLight = 1,
	DirectionalLight = 2,
	Spotlight = 3,
};

struct Light
{
	// TODO: support spotlight and (point light or directional light)
	// Note - you also need to modify struct Light in .glsl files!
	LightType type;
	int enabled = 1;		// Your light should be able to turn on/off independently!
	glm::vec3 pos;
	glm::vec3 illu_amb, illu_dif, illu_spec;
	float angle = 180.f, exponent = 3.f, size = 1.f;

	glm::vec3 lookAt = glm::vec3(0.f);
	Engine::Transform transform;
};

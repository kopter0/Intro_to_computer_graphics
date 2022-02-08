#pragma once

#include <glm/glm.hpp>
#include <Animation.hpp>
#include <Transform.hpp>

#define MAX_LIGHTS 3
// Feel free to increase maximum number of lights!

struct ProjectionData
{
	float aspectRatio = 4.0f / 3.0f;
	float fov = 60.0f;
	float zNear = 0.01f;
	float zFar = 1000.0f;
};

enum LightType : int
{
	PointLight = 1,
	DirectionalLight = 2,
	Spotlight = 3,
};

// Support point light and directional light.
struct Light
{
	LightType type;
	ProjectionData _projection;
	glm::mat4 LightProj;
	int enabled = 1;		// Your light should be able to turn on/off independently!
	glm::vec3 illu_amb, illu_diff, illu_spec;
	int haveShadowMap = 0;
	float angle, exp;
	Engine::Transform transform;
	Animation* animation;
};

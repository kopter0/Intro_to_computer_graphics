#pragma once

#include <glm/glm.hpp>

struct MatProp {
	glm::vec3 amb, diff, spec;
	int shininess;
	float alpha;
};
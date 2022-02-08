#pragma once

#include <Material.hpp>

class DefaultMaterial : public Engine::Material
{
public:
    void CreateMaterial();
    void UpdateColor(glm::vec3 color);
	void UpdateColorOpacity(glm::vec4 color);
};
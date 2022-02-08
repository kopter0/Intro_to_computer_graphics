#pragma once

#include <Light.hpp>
#include <Material.hpp>

class PhongMaterial : public Engine::Material
{
public:
    void CreateMaterial();
	void UpdateShininess(float shininess);
    void UpdateAmbientReflectance(glm::vec3 color);
	void UpdateDiffuseReflectance(glm::vec3 color);
	void UpdateSpecularReflectance(glm::vec3 color);
	void UpdateLight(std::vector<Light> &lights);
	void UpdateFog(glm::vec3 color);
};

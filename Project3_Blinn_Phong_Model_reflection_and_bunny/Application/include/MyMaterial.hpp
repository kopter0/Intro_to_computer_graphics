#pragma once

#include <Material.hpp>
#include <Light.hpp>

class MyMaterial : public Engine::Material
{
private:
    // TODO:
public:
    void CreateMaterial();
	void UpdateShininess(float shininess);
	void UpdateDiffuseReflectance(float coef);
	void UpdateLights(std::vector<Light> lights);
};

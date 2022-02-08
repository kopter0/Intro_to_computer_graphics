#pragma once

#include <Light.hpp>
#include <Material.hpp>
#include <MatPropContainer.hpp>

class NormalMaterial : public Engine::Material
{
private:
    //TODO
	GLuint pid;
public:
	//TODO
    void CreateMaterial(int unit);
	void UpdateReflectanceProperties(MatProp prop);
	void UpdateTextureCubic(int unit);
	void UpdateTexture2D(int unit);
	void UpdateLight(std::vector<Light> lights);
};

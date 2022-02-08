#pragma once

#include <Material.hpp>
#include <Light.hpp>

class ShadowMaterial : public Engine::Material
{
private:
	GLuint pid;
    //TODO
public:
    void CreateMaterial();
	void LightProj(glm::mat4 lightProj);
	void IsPoint(int n);
	//TODO
};

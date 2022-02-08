#include <Material.hpp>

namespace Engine
{
    Material::Material()
    {
        _program = new Program();
    }

    Material::~Material()
    {
        delete _program;
    }

	void Material::UpdateAmbientReflectance(glm::vec3 color) {
	}

	void Material::UpdateDiffuseReflectance(glm::vec3 color) {
	}

	void Material::UpdateSpecularReflectance(glm::vec3 color) {
	}

	void Material::UpdateShininess(float shininess) {
	}
}
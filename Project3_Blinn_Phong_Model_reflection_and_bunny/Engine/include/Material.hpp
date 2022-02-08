#pragma once

#include <glm/glm.hpp>

#include <Program.hpp>

namespace Engine
{
    class Material
    {
    protected:
        Program* _program;
    public:
        Material();
        ~Material();
        Program *GetProgram() { return _program; }
		virtual void UpdateAmbientReflectance(glm::vec3 color);
		virtual void UpdateDiffuseReflectance(glm::vec3 color);
		virtual void UpdateSpecularReflectance(glm::vec3 color);
		virtual void UpdateShininess(float);
    };
}
#pragma once

#include <Material.hpp>

class DefaultMaterial : public Engine::Material
{
private:
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
public:
    void CreateMaterial();
    void UpdateColor(glm::vec4 color);
};
#pragma once

#include <Material.hpp>

class LineMaterial : public Engine::Material
{
private:
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
public:
    void CreateMaterial();
    void SetColor(glm::vec4 mat_color) { color = mat_color; };
};
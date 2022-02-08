#pragma once

#include <iostream>
#include <exception>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Camera.hpp>
#include <Mesh.hpp>
#include <Material.hpp>

namespace Engine
{
    class RenderObject
    {
    private:
        glm::vec3 _position;
        glm::mat4 _orientation;
        glm::vec3 _scale;

    protected:
        Mesh* _mesh;
        Material* _material;

    public:
        RenderObject();
        RenderObject(Mesh* mesh, Material* material);
        void Render(Camera* cam);
        void SetMesh(Mesh* mesh) { _mesh = mesh; }
        void SetMaterial(Material* material) { _material = material; }
        glm::vec3 GetPosition() { return _position; }
        void SetPosition(glm::vec3 position) { _position = position; }
        glm::mat4 GetOrientation() { return _orientation; }
        void SetOrientation(glm::mat4 orientation) { _orientation = orientation; }
        glm::vec3 GetScale() { return _scale; }
        void SetScale(glm::vec3 scale) { _scale = scale; }
        glm::mat4 GetWorldTransform();
    };
}
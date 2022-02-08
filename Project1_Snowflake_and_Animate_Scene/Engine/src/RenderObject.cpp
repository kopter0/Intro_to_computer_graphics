#include <RenderObject.hpp>

namespace Engine
{
    RenderObject::RenderObject() : _position(glm::vec3(0.0f)), _orientation(glm::mat4(1.0f)), _scale(glm::vec3(1.0f, 1.0f, 1.0f))
    {
    }

    RenderObject::RenderObject(Mesh* mesh, Material* material) : _mesh(mesh), _material(material), _position(glm::vec3(0.0f)), _orientation(glm::mat4(1.0f)), _scale(glm::vec3(1.0f, 1.0f, 1.0f))
    {
    }

    void RenderObject::Render(Camera* cam)
    {
        GLuint programId = _material->GetProgram()->GetProgramId();
        glUseProgram(programId);

        // Send camera uniform data to glsl program
        glm::mat4 worldTransform = GetWorldTransform();

        glUniformMatrix4fv(glGetUniformLocation(programId, "worldTransform"), 1, GL_FALSE, (float*)&worldTransform);

        glm::mat4 cameraTransform = cam->GetCameraTransform();
        glUniformMatrix4fv(glGetUniformLocation(programId, "cameraTransform"), 1, GL_FALSE, (float*)&cameraTransform);

        glm::mat4 projectionMatrix = cam->GetProjectionMatrix();
        glUniformMatrix4fv(glGetUniformLocation(programId, "projectionMatrix"), 1, GL_FALSE, (float*)&projectionMatrix);

        GLenum drawMode = _mesh->GetDrawMode();
        int numElements = _mesh->GetNumElements();

        if (drawMode == GL_FALSE)
        {
            std::cout << "Cannot render this object with undefined behaviour\n";
            throw std::exception();
        }

        if (numElements == 0)
        {
            std::cout << "Cannot render this object with zero elements\n";
            throw std::exception();
        }

        glBindVertexArray(_mesh->GetVao());

        if (_mesh->GetHasIndex())
        {
            glBindBuffer(GL_ARRAY_BUFFER, _mesh->GetVbo());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->GetIbo());
            glDrawElements(drawMode, numElements, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, _mesh->GetVbo());
            glDrawArrays(drawMode, 0, numElements);
        }

        glBindVertexArray(0);
    }

    glm::mat4 RenderObject::GetWorldTransform()
    {
        // Compute TRS
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), _position);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);
        return translationMatrix * _orientation * scaleMatrix;
    }
}
#include <DefaultMaterial.hpp>

void DefaultMaterial::CreateMaterial()
{
    _program->AttachShader("Resources\\DefaultMaterial\\VertexShader.glsl", GL_VERTEX_SHADER);
    _program->AttachShader("Resources\\DefaultMaterial\\FragmentShader.glsl", GL_FRAGMENT_SHADER);
    _program->LinkShader();

    glm::vec4 defaultColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glUseProgram(_program->GetProgramId());
    GLuint location = glGetUniformLocation(_program->GetProgramId(), "color");
    glUniform4fv(location, 1, (float*)&defaultColor);
}

void DefaultMaterial::UpdateColor(glm::vec3 color)
{
    glUseProgram(_program->GetProgramId());
    GLuint location = glGetUniformLocation(_program->GetProgramId(), "color");
    glUniform4fv(location, 1, (float*)&glm::vec4(color, 1.0f));
}
// Same as above but changes all 4 colors
void DefaultMaterial::UpdateColorOpacity(glm::vec4 color)
{
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "color");
	glUniform4fv(location, 1, (float*)&color);
}
#include <ShadowMaterial.hpp>
#define BUF_SIZE 64

//TODO
void ShadowMaterial::CreateMaterial()
{
	pid = _program->GetProgramId();
	_program->AttachShader("Resources\\Materials\\ShadowVS.glsl", GL_VERTEX_SHADER);
	_program->AttachShader("Resources\\Materials\\ShadowFS.glsl", GL_FRAGMENT_SHADER);
	_program->LinkShader();
}


void ShadowMaterial::LightProj(glm::mat4 lightProj) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "LightProj");
	glUniformMatrix4fv(location,1, GL_FALSE, (float*)&lightProj);
}


void ShadowMaterial::IsPoint(int n) {
	glUseProgram(pid);
	GLuint location = glGetUniformLocation(pid, "numLights");
	glUniform1i(pid, n);
}
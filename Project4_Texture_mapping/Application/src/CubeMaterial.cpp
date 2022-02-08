#include <CubeMaterial.hpp>
#define BUF_SIZE 64
void CubeMaterial::CreateMaterial(int unit)
{
	_program->AttachShader("Resources\\Materials\\CubeVertexShader.glsl", GL_VERTEX_SHADER);
	_program->AttachShader("Resources\\Materials\\CubeFragmentShader.glsl", GL_FRAGMENT_SHADER);
	_program->LinkShader();

	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "skybox");
	glUniform1i(location, unit);
}

void CubeMaterial::IsSkybox(int is_skybox)
{
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "is_skybox");
	glUniform1i(location, is_skybox);
}


void CubeMaterial::AddNormalMap(int unit) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "normal_map");
	glUniform1i(location, unit);
}

void CubeMaterial::UpdateLight(std::vector<Light> lights) {
	GLuint pid = _program->GetProgramId();
	glUseProgram(pid);
	
	int size = lights.size();
	GLuint location = glGetUniformLocation(pid, "numLights");
	glUniform1i(location, size);

	char buf[BUF_SIZE];

	for (int i = 0; i < size; i++) {
		glm::mat4 wTrans = lights[i].transform.GetWorldTransform();
		glm::vec4 wPos = wTrans * glm::vec4(0, 0, 0, 1);
		snprintf(buf, BUF_SIZE, "lights[%d].pos", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(wPos));
	}
}

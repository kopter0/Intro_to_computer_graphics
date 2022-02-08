#include <PhongMaterial.hpp>

#define BUF_SIZE 64

void PhongMaterial::CreateMaterial()
{
	_program->AttachShader("Resources\\Materials\\VertexShader.glsl", GL_VERTEX_SHADER);
	_program->AttachShader("Resources\\Materials\\PhongFragmentShader.glsl", GL_FRAGMENT_SHADER);
	_program->LinkShader();
}

void PhongMaterial::UpdateAmbientReflectance(glm::vec3 color)
{
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "ambientRef");
	glUniform3fv(location, 1, (float*)&color);
}

void PhongMaterial::UpdateDiffuseReflectance(glm::vec3 color)
{
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "diffuseRef");
	glUniform3fv(location, 1, (float*)&color);
}

void PhongMaterial::UpdateSpecularReflectance(glm::vec3 color)
{
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "specularRef");
	glUniform3fv(location, 1, (float*)&color);
}

void PhongMaterial::UpdateFog(glm::vec3 color) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "fogC");
	glUniform3fv(location, 1, (float*)&color);
}

void PhongMaterial::UpdateShininess(float shininess) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "shininess");
	glUniform1f(location, shininess);
}



void PhongMaterial::UpdateLight(std::vector<Light> &lights)
{
	GLuint pid = _program->GetProgramId();
	glUseProgram(pid);
	
	GLuint location = glGetUniformLocation(pid, "numLights");
	glUniform1i(location, lights.size());

	char buf[BUF_SIZE];
	for (int i = 0; i < lights.size(); i++) {
		Light light = lights[i];
		
		snprintf(buf, BUF_SIZE, "lights[%d].enabled", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1i(location, lights[i].enabled);

		snprintf(buf, BUF_SIZE, "lights[%d].illu_dif", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_dif));

		snprintf(buf, BUF_SIZE, "lights[%d].illu_amb", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_amb));

		snprintf(buf, BUF_SIZE, "lights[%d].illu_spec", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_spec));

		glm::mat4 world_transform = lights[i].transform.GetWorldTransform();
		glm::vec4 local_pos = glm::vec4(0.0, 0.0, 0.0, 1.0);
		glm::vec4 world_pos = world_transform * local_pos;
		snprintf(buf, BUF_SIZE, "lights[%d].pos", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(world_pos));

		snprintf(buf, BUF_SIZE, "lights[%d].lookAt", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].lookAt));

		snprintf(buf, BUF_SIZE, "lights[%d].angle", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1f(location, lights[i].angle);

		snprintf(buf, BUF_SIZE, "lights[%d].exponent", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1f(location, lights[i].exponent);

		snprintf(buf, BUF_SIZE, "lights[%d].type", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1i(location, lights[i].type);

		snprintf(buf, BUF_SIZE, "lights[%d].size", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1f(location, lights[i].size);
	}
}



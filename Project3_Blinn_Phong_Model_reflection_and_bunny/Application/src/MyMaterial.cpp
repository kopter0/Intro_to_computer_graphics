#include <MyMaterial.hpp>

#define BUF_SIZE 64

void MyMaterial::CreateMaterial()
{
	_program->AttachShader("Resources\\Materials\\VertexShader.glsl", GL_VERTEX_SHADER);
	_program->AttachShader("Resources\\Materials\\NightVisionFragmentShader.glsl", GL_FRAGMENT_SHADER);
	_program->LinkShader();
}

void MyMaterial::UpdateShininess(float shininess) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "shininess");
	glUniform1f(location, shininess);
}

void MyMaterial::UpdateDiffuseReflectance(float coef) {
	glUseProgram(_program->GetProgramId());
	GLuint location = glGetUniformLocation(_program->GetProgramId(), "diffuseRef");
	glUniform1f(location, coef);
}

void MyMaterial::UpdateLights(std::vector<Light> lights) {
	
	GLuint pid = _program->GetProgramId();
	glUseProgram(pid);
	int numLights = lights.size();
	GLuint location = glGetUniformLocation(pid, "numLights");
	glUniform1i(location, numLights);

	char buf[BUF_SIZE];
	for (int i = 0; i < numLights; i++) {

		snprintf(buf, BUF_SIZE, "lights[%d].enabled", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1i(location, lights[i].enabled);

		snprintf(buf, BUF_SIZE, "lights[%d].illu_amb", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&lights[i].illu_amb);

		snprintf(buf, BUF_SIZE, "lights[%d].illu_dif", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&lights[i].illu_dif);

		snprintf(buf, BUF_SIZE, "lights[%d].illu_spec", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&lights[i].illu_spec);
		

		glm::mat4 world_transform = lights[i].transform.GetWorldTransform();
		glm::vec4 world_pos = world_transform * glm::vec4(0.f, 0.f, 0.f, 1.f);
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
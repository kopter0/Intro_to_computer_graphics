#include <NormalMaterial.hpp>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BUF_SIZE 64

//TODO
void NormalMaterial::CreateMaterial(int unit)
{
	_program->AttachShader("Resources\\Materials\\NormalVS.glsl", GL_VERTEX_SHADER);
	_program->AttachShader("Resources\\Materials\\NormalFS.glsl", GL_FRAGMENT_SHADER);
	_program->LinkShader();

	pid = _program->GetProgramId();

	glUseProgram(pid);
	GLuint location = glGetUniformLocation(pid, "normalMap");
	glUniform1i(location, unit);
}

void NormalMaterial::UpdateReflectanceProperties(MatProp prop) {
	glUseProgram(pid);
	GLuint location = glGetUniformLocation(pid, "ambRef");
	glUniform3fv(location, 1, (float*)&prop.amb);
	location = glGetUniformLocation(pid, "diffRef");
	glUniform3fv(location, 1, (float*)&prop.diff);
	location = glGetUniformLocation(pid, "specRef");
	glUniform3fv(location, 1, (float*)&prop.spec);
	location = glGetUniformLocation(pid, "shininess");
	glUniform1i(location, prop.shininess);
	location = glGetUniformLocation(pid, "transparency");
	glUniform1f(location, prop.alpha);


}

void NormalMaterial::UpdateTextureCubic(int unit) {
	glUseProgram(pid);
	GLuint location = glGetUniformLocation(pid, "TextureCubic");
	glUniform1i(location, unit);

	location = glGetUniformLocation(pid, "TCubicDef");
	glUniform1i(location, 1);
}

void NormalMaterial::UpdateTexture2D(int unit) {
	glUseProgram(pid);
	GLuint location = glGetUniformLocation(pid, "Texture2D");
	glUniform1i(location, unit);

	location = glGetUniformLocation(pid, "T2DDef");
	glUniform1i(location, 1);
}

void NormalMaterial::UpdateLight(std::vector<Light> lights) {
	pid = _program->GetProgramId();
	glUseProgram(pid);
	size_t size = lights.size();
	GLuint location = glGetUniformLocation(pid, "numLights");
	glUniform1i(location, (int)size);

	char buf[BUF_SIZE];
	for (int i = 0; i < size; i++) {
		snprintf(buf, BUF_SIZE, "lights[%d].type", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1i(location, lights[i].type);
 
		snprintf(buf, BUF_SIZE, "lights[%d].enabled", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1i(location, lights[i].enabled);

		snprintf(buf, BUF_SIZE, "lights[%d].angle", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1f(location, lights[i].angle);

		snprintf(buf, BUF_SIZE, "lights[%d].exp", i);
		location = glGetUniformLocation(pid, buf);
		glUniform1f(location, lights[i].exp);

		glm::mat4 wTrans = lights[i].transform.GetWorldTransform();
		glm::vec4 wPos = wTrans * glm::vec4(0, 0, 0, 1);
		snprintf(buf, BUF_SIZE, "lights[%d].pos", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(wPos));

		glm::vec4 local_dir = glm::vec4(0.0, 0.0, 1.0, 0.0);
		glm::vec4 world_dir = wTrans * local_dir;
		snprintf(buf, BUF_SIZE, "lights[%d].dir", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(world_dir));


		snprintf(buf, BUF_SIZE, "lights[%d].illu_amb", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_amb));

		snprintf(buf, BUF_SIZE, "lights[%d].illu_diff", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_diff));

		snprintf(buf, BUF_SIZE, "lights[%d].illu_spec", i);
		location = glGetUniformLocation(pid, buf);
		glUniform3fv(location, 1, (float*)&(lights[i].illu_spec));

	}
}
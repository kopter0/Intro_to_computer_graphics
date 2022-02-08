#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <PickableObject.hpp>
#include <Geometry.hpp>
#include <DefaultMaterial.hpp>

class Snowman
{
private:
	Engine::Mesh* m_sphere_mesh;
	DefaultMaterial* m_defmat;
	PickingMaterial* m_pick;
	PickableObject *m_head, *m_body, *m_torso;
	int m_elem = 0, m_cur_idx = 1, jumping_phase = 0, poking_phase = 0, angry_phase = 0;
	float toRad = glm::pi<float>() / 180.0f, m_action_counter = 0, m_jump_sign = 1;
	glm::vec3 dir[4] = {
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)
	};
	std::vector<PickableObject*> mainBody, hands, face, hat;
	enum Parts {
		TORSO, BODY, HEAD, ARM = 0, FOREARM
	};
	std::vector<std::pair<glm::vec3, glm::mat4>> BODY_STATE;
	float Y_COMP[3];
	std::vector<std::pair<glm::vec3, glm::mat4>> init_hands;
	std::vector<float> angry_angle = { 0.0f, 0.0f };
#define	Y_AXIS glm::vec3(0.0f, 1.0f, 0.0f)
#define X_AXIS glm::vec3(1.0f, 0.0f, 0.0f)
#define Z_AXIS glm::vec3(0.0f, 0.0f, 1.0f)
public:
    Snowman(float y_comp);
    ~Snowman();
	PickableObject* body;
	PickableObject* head;
	void rotate_head();
	void rotate_body();
	//void update_state();
	void allign();
	void PickingRender(Engine::Camera *cam);
	void Render(Engine::Camera *cam);
	bool jump(float deltaTime, int ddir);
	void symmetricHandsRotation(float f_rot, float a_rot);
	void clench(float dir, float angle);
	void correctHands();
	int poke(float deltaTime);
	bool angry(float deltaTime);
	void angry_rotation(float deltaTime, int in_sign);
	glm::vec3 getPosition();
};
#include <Animation.hpp>
#include <GLFW/glfw3.h>

Animation::Animation()
{
    _objects = std::vector<Engine::RenderObject*>();
}

Animation::~Animation()
{
    _objects.clear();
}

void Animation::AddObject(Engine::RenderObject* object)
{
    _objects.push_back(object);
}

// follwing 3 function used for manipulating trail mode
void Animation::AddTrail(Engine::RenderObject* object) {
	trail.push_back(object);
}

void Animation::RemoveFirst() {
	trail.pop_front();
}

void Animation::trailAnim(float deltaTime) {
	for (Engine::RenderObject* ob : trail) {
		glm::vec3 pos = ob->GetPosition();
		ob->SetPosition(pos - glm::vec3(0.0f, deltaTime, 0.0f));
	}
}

void Animation::Animate(Engine::Camera* cam, float deltaTime)
{

	float slow = 1.5f;
	int count = 0;
	for (Engine::RenderObject* snowflake : _objects) {
		float delta = deltaTime / slow;
		glm::vec3 temp = snowflake->GetPosition();
		glm::vec3 new_pos;
		// if snowflake out of sight move it to top of screen
		if (temp[1] < -2.4f) {
			new_pos = glm::vec3(-3.8f + (std::rand() % 7600) / 1000.0f, 4.3f + (std::rand() % 300) / 100.0f, -(std::rand() % 1000) / 1000.f);
		} // otherwise move by little bit in y-coordinate, and and oscilatte in x-coor
		else
			new_pos = temp - (delta * glm::vec3(cos(glfwGetTime() - count), 1.0f, 0.0f));

		snowflake->SetPosition(new_pos);

		// for rotation just setOrientation
		snowflake->SetOrientation(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)));

		// increment slow so that next snoflake move at different speed
		slow += 0.05f;
		count++;
	}

}


bool Animation::TreeInteraction(Engine::RenderObject* tree, float deltaTime, glm::vec3 direction) {
	counter += deltaTime / toRad;
	switch (MODE)
	{
	case 0:
		// Unless tree has fallen by 90 degree keep rotating
		tree->SetOrientation(glm::rotate(glm::mat4(1.0f), counter * toRad, direction));
		if (counter > 90) {
			MODE++;
			counter = 0;
		}
		break;
	case 1:
		//gradually tree sink in ground
		tree->SetPosition(tree->GetPosition() - glm::vec3(0.0f, deltaTime, 0.0f));
		if (counter * toRad > 1.5) {
			MODE++;
			counter = 0;
		}
		break;
	case 2:
		// orient tree upright and put at some random place
		tree->SetOrientation(glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f)));
		tree->SetPosition(glm::vec3(std::rand() % 10 - 5.0f, -6.0f, std::rand() % 9 - 7));
		MODE++;
		counter = 0;
		break;
	case 3:
		// gradually make tree reappear
		tree->SetPosition(tree->GetPosition() + glm::vec3(0.0f, deltaTime, 0.0f));
		if (counter * toRad > 4) {
			MODE = 0;
			counter = 0;
			return false;
		}
	default:
		break;
	}
	return true;

}
#include <Animation.hpp>

Animation::Animation()
{
    _snowflakes = std::vector<Snowflake*>();
}

Animation::~Animation()
{
    _snowflakes.clear();
}

void Animation::AddSnowflake(Snowflake* snowflake)
{
    _snowflakes.push_back(snowflake);
}

void Animation::Animate(float deltaTime)
{
	float slow = 1.5f;
	int count = 0;
	for (Snowflake* snowflake : _snowflakes) {
		float delta = deltaTime / slow;
		glm::vec3 temp = snowflake->GetPosition();
		glm::vec3 new_pos;
		// if snowflake out of sight move it to top of screen
		if (temp[1] < -4.8f) {
			new_pos = glm::vec3(-3.8f + (std::rand() % 7600) / 1000.0f, 4.3f + (std::rand() % 700) / 100.0f, -(std::rand() % 1000) / 1000.f);
		} // otherwise move by little bit in y-coordinate, and and oscilatte in x-coor
		else
			new_pos = temp - (delta * glm::vec3(cos(glfwGetTime() - count), 1.0f, 0.0f));

		snowflake->SetPosition(new_pos);

		// for rotation just setOrientation
		snowflake->SetOrientation(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() , glm::vec3(0.0f, 0.0f, 1.0f)));
		
		// increment slow so that next snoflake move at different speed
		slow += 0.05f;
		count++;
	}
}



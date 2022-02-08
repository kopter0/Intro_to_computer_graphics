#include <Snowflake.hpp>

/*
 * TODO: Inherit RenderObject class to implement animation
 */

Snowflake::Snowflake(Engine::Mesh* mesh, Engine::Material* material) : Engine::RenderObject(mesh, material)
{
	// In this function I just give them some random location and scale so that they appear smaller
	this->SetPosition(glm::vec3(-3.8f + (std::rand() % 7600)/1000.0f , 4.3f + (std::rand() % 700) / 100.0f, -(std::rand() % 1000) / 1000.f));
    this->SetScale(glm::vec3(0.3f, 0.3f, 0.0f));
}

// Add more functions
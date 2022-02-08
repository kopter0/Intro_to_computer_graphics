#pragma once

#include <vector>
#include <deque>
#include <RenderObject.hpp>

/*
 * Animation class supports continuous animation by changing RenderObject's transform with elasped time in seconds
 */

class Animation
{
private:
    std::vector<Engine::RenderObject*> _objects;
	std::deque<Engine::RenderObject*> trail;
	int MODE = 0;
	float counter = 0, toRad = glm::pi<float>() / 180.0f;
public:
    Animation();
    ~Animation();
	void AddTrail(Engine::RenderObject* object);
	void RemoveFirst();
	void trailAnim(float deltaTime);
    void AddObject(Engine::RenderObject* object);
    void Animate(Engine::Camera* cam, float deltaTime);
	bool TreeInteraction(Engine::RenderObject* tree, float deltaTime, glm::vec3 direction);
};
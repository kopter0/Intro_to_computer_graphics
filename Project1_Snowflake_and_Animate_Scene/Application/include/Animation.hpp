#pragma once

#include <vector>
#include <Snowflake.hpp>
#include <math.h>
#include <GLFW/glfw3.h>
/*
 * Animation class supports continuous animation by changing RenderObject's transform with elasped time in seconds
 */

class Animation
{
private:
    std::vector<Snowflake*> _snowflakes;
 public:
    Animation();
    ~Animation();
    void AddSnowflake(Snowflake* snowflake);
    void Animate(float deltaTime);
};
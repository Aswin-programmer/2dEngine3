#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Time
{
public:
	static float timeStarted;
	static float getTime();
};


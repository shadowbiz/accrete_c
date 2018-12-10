#include "timer.h"

float getTime()
{
	return glfwGetTime();
}

float getTimerDelta()
{
	static double zeroTime = 0;
	double currentTime = getTime();
	double deltaTime = currentTime - zeroTime;
	zeroTime = currentTime;
	return (float)deltaTime;
}

float getTimerFPS(const float deltaTime)
{
	static float timeGone;
	static unsigned int fpsFrameCount = 0;
	static float fps = 0.0;
	static const float timeInterval = 1.0;

	timeGone += deltaTime;

	if (timeGone > timeInterval)
	{
		fps = (float)fpsFrameCount / (timeGone);
		fpsFrameCount = 0;
		timeGone = 0;
	}
	else
	{
		fpsFrameCount++;
	}

#ifdef VERBOSE
	static float timer = 0.0;
	timer += deltaTime;
	if (timer > 5.0f)
	{
		printf("FPS: \t%4.2f \n", fps);
		timer = 0;
	}
#endif

	return fps;
}
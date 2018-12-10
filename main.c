#include "windows.h"
#include "game.h"

static char windowTitle[30];

bool running = true;

inline static int updateLoop(void)
{
	static float accumulator = 0;
	static float currentTime = 0;
	static float fps = 0;
	static float fpsTimer = 0;
	float newTime = glfwGetTime();
	float deltaTime = newTime - currentTime;

	running = true;

	if (deltaTime <= 0.0)
	{
		return 0;
	}

	currentTime = newTime;
	accumulator += deltaTime;

	while (accumulator >= DELTA_TIME)
	{
		if (!EngineUpdate())
		{
			running = false;
			return 0;
		}
		else
		{
			GameUpdate();
		}
		accumulator -= DELTA_TIME;
	}

	fps = getTimerFPS(getTimerDelta());

	fpsTimer += deltaTime;
	if (fpsTimer > 1.5)
	{
		sprintf(windowTitle, "FPS: %.2f", fps);
		glfwSetWindowTitle(windowTitle);
		fpsTimer = 0;
	}

	return 0;
}

inline static void renderLoop(void)
{
	EngineRenderPre();
	EngineRenderPost();
	glfwSwapBuffers();
}

bool init()
{
	printf("Game init...\n");
	engineInitialized = false;
	GLFWvidmode vidMode;
	int screenWidth, screenHeight;

	if (!glfwInit())
	{
		printf("GLFW Init fail...\n");
		return false;
	}

	glfwGetDesktopMode(&vidMode);
	screenWidth = vidMode.width;
	screenHeight = vidMode.height;

	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (FULL_SCREEN)
	{
		glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 32, 32, GLFW_FULLSCREEN);
	}
	else
	{
		glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
		glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 32, 32, GLFW_WINDOW);
	}

	glfwSetWindowPos((screenWidth - WINDOW_WIDTH) / 2, (screenHeight - WINDOW_HEIGHT) / 2);

	sprintf(windowTitle, "Title");
	glfwSetWindowTitle(windowTitle);
	glfwSwapInterval(VSYNC);

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init fail...\n");
		return false;
	}

	printf("GL Vendor: \t%s\n", glGetString(GL_VENDOR));
	printf("GL Renderer: \t%s\n", glGetString(GL_RENDERER));
	printf("GL Version: \t%s\n", glGetString(GL_VERSION));
	printf("GLSL Version: \t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GLint maxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);
	printf("Max supported patch vertices \t%d\n", maxPatchVertices);

	if (!EngineInit())
	{
		printf("Engine init fail...\n");
		return false;
	}

	engineInitialized = true;

	GameInit();

	glfwEnable(GLFW_STICKY_KEYS);
	glfwEnable(GLFW_KEY_REPEAT);

	glfwSetKeyCallback(InputKeyboard);
	glfwSetMouseButtonCallback(InputMouse);

	//glfwDisable(GLFW_MOUSE_CURSOR);

	glfwSetMousePos(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	printf(" OK\n");
	return true;
}

bool getErrorStatus()
{
	printf("Getting errors status...\n");
	printf("->Game release...\n");
	GameRelease();
	printf("->Engine release...\n");
	EngineRelease();

	return (
		getInputAllocateStatus() &&
		getCameraAllocStatus() &&
		getLightAllocateStatus() &&
		getStageAllocateStatus() &&
		getShaderAllocStatus() &&
		getMeshDataAllocateStatus() &&
		getMeshAllocateStatus() &&
		getEntityAllocStatus());
}

void end(void)
{
	glfwEnable(GLFW_MOUSE_CURSOR);

	bool noErrors = getErrorStatus();

	if (!noErrors)
	{
		const float waitTime = 5.1;
		float time = glfwGetTime() + waitTime;
		while (glfwGetTime() < time)
			;
	}

	glfwTerminate();
}

void printMat4(const mat4 *mat)
{
	for (int i = 0; i < 16; i++)
	{
		printf("%2.2f ", mat->data[i]);
		if (((i + 1) % 4) == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

int main(void)
{
	getchar();
	return 0;

	running = init();

	while (glfwGetWindowParam(GLFW_OPENED) && running)
	{
		updateLoop();
		renderLoop();
	}

	if (engineInitialized == true)
	{
		end();
		exit(EXIT_SUCCESS);
	}
	else
	{
		exit(EXIT_FAILURE);
	}

	return 0;
}

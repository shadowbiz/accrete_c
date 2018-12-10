#include "input.h"

bool showMenu = false;

static uint allocs = 0;
static Input *activeInput = NULL;

void setActiveInput(Input *input)
{
	activeInput = input;
}

InputConfig *newInputConfig()
{
	InputConfig *result = seNew(InputConfig, 1);

	if (!result)
		exit(EXIT_FAILURE);

	result->exitCode = GLFW_KEY_ESC;
	result->actionCode = GLFW_KEY_SPACE;
	result->special1Code = GLFW_KEY_LSHIFT;
	result->special2Code = GLFW_KEY_LALT;
	result->upCode = 'W';
	result->downCode = 'S';
	result->leftCode = 'A';
	result->rightCode = 'D';

	return result;
}

void freeInputConfig(InputConfig *config)
{
	if (!config)
		return;
	printf("Destroying input config...\n");
	free(config);
	config = NULL;
}

Input *newInput()
{
	Input *result = seNew(Input, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	allocs++;

	printf("Creating input...\n");

	result->inputConfig = newInputConfig();

	result->downPressed = false;
	result->upPressed = false;
	result->leftPressed = false;
	result->rightPressed = false;
	result->leftMouse = false;
	result->rightMouse = false;
	result->middleMouse = false;
	result->actionPressed = false;
	result->exitPressed = false;
	result->special1Pressed = false;
	result->special2Pressed = false;
	result->isActive = false;

	result->keysPressed = 0;
	result->pressTime = 0;

	result->lastX = 0;
	result->lastY = 0;
	result->lastZ = 0;

	result->mouseDeltaX = 0;
	result->mouseDeltaY = 0;
	result->mouseDeltaZ = 0;

	result->mouseX = 0;
	result->mouseY = 0;

	result->mouseWorldPosition = vec3Allocate(1);
	result->mouseScreenPosition = vec3Allocate(1);

	vec3SetZero(result->mouseWorldPosition);
	vec3SetZero(result->mouseScreenPosition);

	result->Update = InputUpdate;

	setActiveInput(result);
	return result;
}

void freeInput(Input *input)
{
	if (!input)
		return;

	printf("Destroying input...\n");
	freeInputConfig(input->inputConfig);
	freeVec3(input->mouseScreenPosition);
	freeVec3(input->mouseWorldPosition);

	free(input);
	input = NULL;
	allocs--;
}

void getMousePosition(Input *inputData)
{
	int x, y;
	bool mousePosCorrected = false;

	Input *input = inputData;

	input->mouseDeltaX = 0;
	input->mouseDeltaY = 0;
	input->mouseDeltaZ = 0;

	glfwGetMousePos(&x, &y);

	const byte edgeTriggerDistance = 17;
	const byte translationAmount = 12;

	if (x < edgeTriggerDistance)
	{
		if (x < 0)
		{
			x = 0;
			mousePosCorrected = true;
		}
		input->mouseDeltaX = translationAmount;
	}
	else if (x > WINDOW_WIDTH - edgeTriggerDistance)
	{
		if (x > WINDOW_WIDTH)
		{
			x = WINDOW_WIDTH;
			mousePosCorrected = true;
		}
		input->mouseDeltaX = -translationAmount;
	}
	if (y < edgeTriggerDistance)
	{
		if (y < 0)
		{
			y = 0;
			mousePosCorrected = true;
		}
		input->mouseDeltaY = translationAmount;
	}
	else if (y > WINDOW_HEIGHT - edgeTriggerDistance)
	{
		if (y > WINDOW_HEIGHT)
		{
			y = WINDOW_HEIGHT;
			mousePosCorrected = true;
		}
		input->mouseDeltaY = -translationAmount;
	}

	if (mousePosCorrected == true)
	{
		glfwSetMousePos(x, y);
	}

	input->mouseX = (float)x;
	input->mouseY = (float)y;

	int wheel = glfwGetMouseWheel();

	if (wheel != input->lastZ)
	{
		input->mouseDeltaZ = (wheel - input->lastZ) * 40;
		input->lastZ = wheel;
	}

	bool mouseIsActive = input->leftMouse || input->rightMouse || input->middleMouse;

	if (mouseIsActive)
	{
		input->pressTime += DELTA_TIME;
	}

	input->isActive = mouseIsActive || (activeInput->keysPressed > 0) || input->mouseDeltaX || input->mouseDeltaY || input->mouseDeltaZ;

	if (showMenu)
	{
		//printf("Show Menu %2.2f\n", input->pressTime);
		showMenu = false;
	}
	else
	{
		if (activeInput->leftMouse && input->special2Pressed)
		{
			input->mouseDeltaY = (y - input->lastY) * 3.0;
			input->mouseDeltaX = (x - input->lastX) * 3.0;

			input->lastX = x;
			input->lastY = y;
		}

		if (input->rightMouse)
		{
			input->mouseDeltaY = (y - input->lastY);
			input->mouseDeltaX = (x - input->lastX);

			input->lastX = x;
			input->lastY = y;
		}
	}

	if (input->middleMouse)
	{
		input->mouseDeltaZ = (x - input->lastX) - (y - input->lastY);

		input->lastX = x;
		input->lastY = y;
	}
}

bool InputUpdate(Input *input)
{
	if (input->exitPressed)
	{
		return false;
	}

	getMousePosition(input);

	if (input->upPressed)
	{
		input->mouseDeltaY = 10;
	}
	else if (input->downPressed)
	{
		input->mouseDeltaY = -10;
	}
	if (input->leftPressed)
	{
		input->mouseDeltaX = 10;
	}
	else if (input->rightPressed)
	{
		input->mouseDeltaX = -10;
	}

	return true;
}

void GLFWCALL InputKeyboard(int key, int action)
{
	if (!activeInput)
	{
		return;
	}

	if (activeInput->exitPressed)
	{
		return;
	}

	Input *input = activeInput;
	InputConfig *config = activeInput->inputConfig;

	if (key == config->exitCode)
	{
		input->exitPressed = true;
	}
	else if (key == config->special1Code)
	{
		if (action == GLFW_PRESS)
		{
			if (input->special1Pressed != true)
			{
				input->special1Pressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->special1Pressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->special2Code)
	{
		if (action == GLFW_PRESS)
		{
			if (input->special2Pressed != true)
			{
				input->special2Pressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->special2Pressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->actionCode)
	{
		if (action == GLFW_PRESS)
		{
			if (input->actionPressed != true)
			{
				input->actionPressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->actionPressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->upCode)
	{
		if (action == GLFW_PRESS)
		{
			if (input->upPressed != true)
			{
				input->upPressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->upPressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->downCode)
	{
		if (action == GLFW_PRESS)
		{
			if (input->downPressed != true)
			{
				input->downPressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->downPressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->leftCode)
	{
		if (action == GLFW_PRESS)
		{
			if (input->leftPressed != true)
			{
				input->leftPressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->leftPressed = false;
			input->keysPressed--;
		}
	}
	else if (key == config->rightCode)
	{
		if (action == GLFW_PRESS)
		{
			if (input->rightPressed != true)
			{
				input->rightPressed = true;
				input->keysPressed++;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			input->rightPressed = false;
			input->keysPressed--;
		}
	}

#ifdef DEBUG
	printf("%c %i\n", key, input->keysPressed);
#endif // DEBUG
}

void GLFWCALL InputMouse(int key, int action)
{
	if (!activeInput)
	{
		return;
	}

	if (key == GLFW_MOUSE_BUTTON_2)
	{
		activeInput->rightMouse = (action == GLFW_PRESS);
		activeInput->leftMouse = false;
		activeInput->middleMouse = false;
		int x, y;
		glfwGetMousePos(&x, &y);

		activeInput->lastX = x;
		activeInput->lastY = y;

		if (action == GLFW_RELEASE)
		{
			if (activeInput->pressTime <= 0.1)
			{
				showMenu = true;
			}
		}

		activeInput->pressTime = 0.0;
	}
	else if (key == GLFW_MOUSE_BUTTON_3)
	{
		activeInput->middleMouse = (action == GLFW_PRESS);
		activeInput->leftMouse = false;
		activeInput->rightMouse = false;

		int x, y;
		glfwGetMousePos(&x, &y);
		activeInput->lastX = x;
		activeInput->lastY = y;

		activeInput->pressTime = 0.0;
	}
	else if (key == GLFW_MOUSE_BUTTON_1)
	{
		activeInput->leftMouse = (action == GLFW_PRESS);
		activeInput->rightMouse = false;
		activeInput->middleMouse = false;

		int x, y;
		glfwGetMousePos(&x, &y);
		activeInput->lastX = x;
		activeInput->lastY = y;

		activeInput->pressTime = 0.0;
	}
}

bool getInputAllocateStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Input allocations left %i\n", allocs);
		return false;
	}
	return true;
}
#ifndef SE_INPUT_H
#define SE_INPUT_H

#include "structs.h"

static InputConfig *inputConfigAllocate();
InputConfig *newInputConfig();
void freeInputConfig(InputConfig *config);

static Input *InputAllocate();
Input *newInput();
void freeInput(Input *input);

void GLFWCALL InputKeyboard(int key, int action);
void GLFWCALL InputMouse(int key, int action);
void GLFWCALL InputMousePosition(int x, int y);

bool InputUpdate(Input *input);
bool getInputAllocateStatus();

#endif
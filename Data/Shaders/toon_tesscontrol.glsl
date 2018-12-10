#version 410 core

layout (vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];

out vec3 tcPosition[];
out vec3 tcNormal[];

#define ID gl_InvocationID

int tessLevel = 3;

void main()
{
	tcPosition[ID] = vPosition[ID];
	tcNormal[ID] = vNormal[ID];

	if (ID == 0)
	{
		gl_TessLevelInner[0] = tessLevel;
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
	}
	
}
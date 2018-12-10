#version 330

layout(location = 0) in vec3 VertexPosition;
layout(location = 2) in vec2 VertexTexCoord;

const float pi = 3.14159265358;

uniform mat4 ModelViewMatrix;
uniform mat4 ProjMatrix;

uniform int Type;
uniform float Delta;

out vec4 starColor;

vec4 GetStarColor(int type)
{
	switch (type)
	{
		case 0:
			return vec4(0.2, 0.6, 1, 1);

		case 1:
			return vec4(0.6, 0.7, 1, 1);

		case 2:
			return vec4(0.95, 0.95, 1, 1);

		case 3:
			return vec4(1, 1, 0.7, 1);

		case 4:
			return vec4(1, 1, 0.3, 1);

		case 5:
			return vec4(1, 0.7, 0.1, 1);

		case 6:
			return vec4(0.9, 0.1, 0, 1);
	}

	return vec4(0,1,0,1);
}

void main()
{
	starColor = GetStarColor(Type);	
	mat4 MVP = ProjMatrix * ModelViewMatrix;
	gl_Position = MVP * vec4(VertexPosition,1.0);
}


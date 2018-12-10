#version 410 core
 
layout(triangles, equal_spacing, ccw) in;

//uniform mat4 ModelViewProjectionMatrix;
//uniform mat4 ModelViewMatrix;

in vec3 tcPosition[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec3 teNormal;

vec3 getAvarage(const vec3 v1, const vec3 v2, const vec3 v3)
{
	return vec3(gl_TessCoord.x) * v1 + vec3(gl_TessCoord.y) * v2 + vec3(gl_TessCoord.z) * v3;
}

void main()
{
	tePosition = getAvarage(tcPosition[0], tcPosition[1], tcPosition[2]);
	teNormal = getAvarage(tcNormal[0], tcNormal[1], tcNormal[2]);

	gl_Position = vec4(tePosition, 1);
}
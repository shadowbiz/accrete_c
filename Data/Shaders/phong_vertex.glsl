#version 410 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

struct LightInfo
{
	vec3 Position;	// Light position in eye coords.
	vec3 La;		 // Ambient light intensity
	vec3 Ld;		// Diffuse light intensity
	vec3 Ls;		// Specular light intensity
};

uniform LightInfo Light;

uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform mat3 NormalMatrix;

out vec3 vPosition;
out vec3 vLightDirection;
out vec3 vEyeVector;
out vec3 vNormal;

void main()
{
	vPosition = vec3(vec4(VertexPosition,1) * ModelViewMatrix);
	vLightDirection = vec3(vec4(Light.Position,1)) - vPosition;
	vEyeVector = -vPosition;
	vNormal = normalize(NormalMatrix * VertexNormal);

	gl_Position = ModelViewProjectionMatrix * vec4(VertexPosition,1);
}
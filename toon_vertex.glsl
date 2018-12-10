#version 330

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

struct LightInfo
{
	vec3 Position;	// Light position in eye coords.
	vec3 La;		// Ambient light intensity
	vec3 Ld;		// Diffuse light intensity
	vec3 Ls;		// Specular light intensity
};

uniform LightInfo Light;

uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelViewProjectionMatrix;
uniform mat3 NormalMatrix;

out LightInfo v_Light;

out vec3 LightDirection;
out vec3 EyeVector;

out vec3 Position;
out vec3 Normal;

out vec2 TexCoord;

void main()
{
	TexCoord = VertexTexCoord;

	v_Light.Position = (vec4(Light.Position,1) * ViewMatrix).xyz;
	v_Light.La = Light.La;
	v_Light.Ld = Light.Ld;
	v_Light.Ls = Light.Ls;

	Normal = normalize(VertexNormal * NormalMatrix);
	Position = vec3(vec4(VertexPosition,1.0) * ModelViewMatrix );
	
	//Normal = normalize(NormalMatrix * VertexNormal);
	//Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
	
	EyeVector = -Position;

	LightDirection = v_Light.Position - Position;
	
	gl_Position = ModelViewProjectionMatrix * vec4(VertexPosition,1.0);
}
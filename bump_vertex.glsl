#version 330

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;
layout(location = 3) in vec3 VertexTangent;

struct LightInfo
{
	vec3 Position;	// Light position in eye coords.
	vec3 La;		// Ambient light intensity
	vec3 Ld;		// Diffuse light intensity
	vec3 Ls;		// Specular light intensity
};

uniform LightInfo Light;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjMatrix;
uniform mat3 NormalMatrix;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;

void main()
{
	mat4 MVP = ProjMatrix * ModelViewMatrix;

	vec3 norm = normalize(NormalMatrix * VertexNormal);
	vec3 tang = normalize(NormalMatrix * VertexTangent);
	vec3 bitang = normalize(cross(tang, norm));

	mat3 toObjectLocal = mat3(	tang.x, bitang.x, norm.x,
								tang.y, bitang.y, norm.y,
								tang.z, bitang.z, norm.z ) ;

	vec3 pos = vec3(ModelViewMatrix * vec4(VertexPosition,1.0));

	LightDir = normalize(toObjectLocal * (vec3(ViewMatrix * vec4(Light.Position,1)) - pos));
	ViewDir = toObjectLocal * normalize(-pos);

	TexCoord = VertexTexCoord;

	gl_Position = MVP * vec4(VertexPosition,1.0);
}
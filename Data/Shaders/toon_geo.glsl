#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

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
//uniform mat3 NormalMatrix;

in vec3 tePosition[3];
in vec3 teNormal[3];

out vec3 gPosition;
out vec3 gNormal;

out LightInfo gLight;

out vec3 gLightDirection;
out vec3 gEyeVector;

void main()
{
	gLight.Position = vec3(ViewMatrix * vec4(Light.Position,1));
	//gLight.Position = vec3(ViewMatrix * vec4(0,100,100,1));
	gLight.La = Light.La;
	gLight.Ld = Light.Ld;
	gLight.Ls = Light.Ls;

    gNormal = teNormal[0];
    gPosition = vec3(vec4(tePosition[0],1) * ModelViewMatrix);
    gEyeVector = -tePosition[0];
    gLightDirection = gLight.Position - gPosition;
    gl_Position = ModelViewProjectionMatrix * gl_in[0].gl_Position;

    EmitVertex();

	gNormal = teNormal[1];
	gPosition = vec3(vec4(tePosition[1],1) * ModelViewMatrix);
	gEyeVector = -tePosition[1];
	gLightDirection = gLight.Position - gPosition;
    gl_Position = ModelViewProjectionMatrix * gl_in[1].gl_Position;

    EmitVertex();

    gNormal = teNormal[2];
    gPosition = vec3(vec4(tePosition[2],1) * ModelViewMatrix);
    gEyeVector = -tePosition[2];
    gLightDirection = gLight.Position - gPosition;
    gl_Position = ModelViewProjectionMatrix * gl_in[2].gl_Position;

    EmitVertex();

    EndPrimitive();
}
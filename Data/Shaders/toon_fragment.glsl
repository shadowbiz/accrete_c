#version 330

struct LightInfo
{
	vec3 Position;	// Light position in eye coords.
	vec3 La;		 // Ambient light intensity
	vec3 Ld;		// Diffuse light intensity
	vec3 Ls;		// Specular light intensity
};

uniform sampler2D u_diffuseMap;

in LightInfo v_Light;

in vec3 LightDirection;
in vec3 EyeVector;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 fragColor;

const int levels = 5;
const float scaleFactor = 1.0 / levels;

vec3 toonShade()
{
	vec4 color = vec4(1);//texture2D(u_diffuseMap, TexCoord);
	vec3 s = normalize(LightDirection);
	float cosine = max( 0.0, dot( s, Normal ) );
	vec3 diffuse = vec3(0.85) * floor( cosine * levels ) * scaleFactor;
	return vec3(0.9) * (vec3(0.1) + diffuse * color.rgb);
}

void main()
{
	fragColor = vec4(toonShade(),1);
}
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

const int levels = 3;
const float scaleFactor = 1.0 / levels;

vec3 toonShade()
{
	vec4 color = vec4(1);//texture2D(u_diffuseMap, TexCoord);
	vec3 s = normalize(LightDirection);
	float cosine = max( 0.0, dot( s, Normal ) );
	vec3 diffuse = v_Light.Ld * floor( cosine * levels ) * scaleFactor;
	return vec3(0.9) * (v_Light.La + diffuse * color.rgb);
}

vec3 ads()
{
	vec4 color = vec4(1);//texture2D(u_diffuseMap, TexCoord);
	
	//vec3 n = normalize(Normal);
	vec3 n = Normal;
	vec3 s = normalize(LightDirection);
	vec3 v = normalize(EyeVector);
	vec3 h = normalize(v + s);

	vec3 ambient = color.rgb * vec3(0.1);
	vec3 diffuse = color.rgb * vec3(0.85) * max( dot(s, Normal), 0.0 );
	vec3 spec = color.rgb * vec3(0.6) * pow(max(dot(h,n), 0.0), 50);
	return ambient + diffuse + spec;
}

void main()
{
	//fragColor = vec4(v_Light.La, 1.0);//color;
	//fragColor = vec4(toonShade(),1) + vec4(ads(), 1.0) * 0.5;
	fragColor = vec4(ads(), 1.0);//color;
	//fragColor = vec4(1,0,0, 1.0);//color;
}
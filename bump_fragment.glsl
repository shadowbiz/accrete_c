#version 400

in vec3 LightDir;
in vec2 TexCoord;
in vec3 ViewDir;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;

struct LightInfo
{
	vec3 Position;	// Light position in eye coords.
	vec3 La;		 // Ambient light intensity
	vec3 Ld;		// Diffuse light intensity
	vec3 Ls;		// Specular light intensity
};

uniform LightInfo Light;

layout( location = 0 ) out vec4 FragColor;

vec3 phongModel(vec3 norm, vec3 diffR)
{
	vec3 r = reflect(-LightDir, norm);
	float sDotN = max(dot(LightDir, norm), 0.0);
	
	vec3 ambient = diffR * Light.La;
	vec3 diffuse = diffR * sDotN;
	vec3 spec = vec3(0.0);
	
	if(sDotN > 0.0) 
	{
		spec = Light.Ls * pow(max(dot(r,ViewDir), 0.0 ), 60);
	}
	return ambient + diffuse + spec;
}

void main()
{
	vec4 normal = normalize(2 * texture(u_normalMap, TexCoord) - 1);
	vec4 texColor = texture(u_diffuseMap, TexCoord);

	FragColor = vec4(phongModel(normal.xyz, texColor.rgb), 1.0 );
}
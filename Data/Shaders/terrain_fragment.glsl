#version 400

in vec2 texcoord;
in float depth;

out vec4 fragmentColor;

uniform sampler2D diffuseMap;
uniform sampler2D heightMap;
uniform sampler2D noise_tile;

vec3 incident = normalize(vec3(1.0, 0.2, 0.5));
vec4 light = vec4(1.0, 0.95, 0.9, 1.0) * 1.1;

void main()
{
	vec3 normal = normalize(texture(heightMap, texcoord).xyz);
	vec4 color = texture(diffuseMap, texcoord);
	
	fragmentColor = color;
	return;

	float noise_factor = 1;//texture(noise_tile, texcoord*32).r+0.1;

	float dot_surface_incident = max(0, dot(normal, incident));

	color = color * light * noise_factor * (max(0.1, dot_surface_incident)+0.05)*1.5;

	fragmentColor = mix(color, color*0.5+vec4(0.5, 0.5, 0.5, 1.0), depth*2.0);
}
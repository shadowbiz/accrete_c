#version 330 core

in vec2 TexCoord;
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main()
{
	//color = texture2D(myTextureSampler, TexCoord);
	color = vec4(1,1,0,1);
}
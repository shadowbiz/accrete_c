#version 400

layout(location = 0) in vec2 VertexPosition;
layout(location = 2) in vec2 VertexTexCoord;

out vec2 Texcoord;
out vec4 pos;

void main()
{
    Texcoord = VertexTexCoord;
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
	pos = gl_Position;
}
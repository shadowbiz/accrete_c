#version 400

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

uniform sampler2D heightMap;

void main(void)
{
    vec2 texcoord = VertexPosition.xy;
    float height = texture(heightMap, texcoord).a;
    vec4 displaced = vec4(VertexPosition.x, VertexPosition.y, height, 1.0);
    gl_Position = displaced;
}
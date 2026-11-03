#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(VertexPos, 1.0);
	Color = VertexColor;
}
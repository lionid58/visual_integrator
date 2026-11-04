#version 330 core

in vec3 Color;

out vec4 FragColor;
uniform float AmbientLight;

void main()
{
	FragColor = vec4(AmbientLight * Color, 1.0f);
}
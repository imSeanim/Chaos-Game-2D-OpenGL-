#version 330 core

in vec3 ourColor;
out vec4 FragColor;

uniform float time;
void main()
{
	// Shader source by Shadertoy.com: https://www.shadertoy.com/new
	vec3 col = 0.5 + 0.35*cos(time + ourColor.xyx + vec3(0,2,4));
	FragColor = vec4(col, 1.0);
}
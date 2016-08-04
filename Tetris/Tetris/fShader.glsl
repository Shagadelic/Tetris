#version 330 core
#define PI 3.141529

in vec3 curColor;

out vec4 color;

uniform float time, blockColor;

void main()
{
    color = vec4(sin(mod(time * blockColor, 2 * PI)), sin(mod(.2 * blockColor * time, 2 * PI)), sin(mod(time, 2 * PI)), 1.0f) *
			vec4(curColor, 1.0);
} 
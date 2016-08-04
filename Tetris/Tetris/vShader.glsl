#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform vec2 pos;

out vec3 curColor;

void main()
{
    gl_Position = mat4(vec4(1, 0, 0, 0),
					vec4(0, 1, 0, 0),
					vec4(0, 0, 1, 0),
					vec4(pos.x * 2 - .9f, 1.0f - pos.y, 1, 1)) * vec4(position, 1.0);
	curColor = color;
}
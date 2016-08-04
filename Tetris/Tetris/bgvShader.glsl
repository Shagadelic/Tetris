#version 330 core
  
layout (location = 2) in vec2 tCoord; //2D TEXTURE
layout (location = 3) in vec3 position; //unmodified position

out vec2 texCoord;

void main()
{
    gl_Position = vec4(position.x, -position.y, position.z, 0.75);	
	texCoord = tCoord;
}
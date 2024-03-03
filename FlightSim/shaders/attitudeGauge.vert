#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoordIn;

uniform mat4 projMatrix;

out vec2 texCoord;

void main()
{
	texCoord = texCoordIn;
	gl_Position = projMatrix * vec4(position, 1.0);
}

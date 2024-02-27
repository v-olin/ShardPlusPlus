#version 420

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normalIn;
layout (location = 2) in vec2 texCoordIn;

uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjMatrix;

out vec2 texCoord;
out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;

void main() {
	gl_Position = modelViewProjMatrix * vec4(position, 1.0f);
	viewSpaceNormal = (normalMatrix * vec4(normalIn, 0.0f)).xyz;
	viewSpacePosition = (modelViewMatrix * vec4(position, 1.0f)).xyz;
	texCoord = texCoordIn;
}
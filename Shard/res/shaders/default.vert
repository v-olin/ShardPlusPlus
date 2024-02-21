#version 420

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform vec3 u_ViewPosition;
uniform mat4 u_ModelMatrix;
uniform mat4 u_MVP;
uniform mat4 u_ViewMatrix;

out vec3 positionWorldSpace;
out vec3 viewPositionWorldSpace;
out vec3 normal_;
out vec3 viewSpaceNormal_;
out vec2 texCoord_;
out vec3 viewPositionViewSpace;

void main() {
	gl_Position = u_MVP * vec4(position, 1.0);
	positionWorldSpace = (u_ModelMatrix * vec4(position, 1.0)).xyz;
	viewPositionWorldSpace = (u_ModelMatrix * vec4(u_ViewPosition, 1.0)).xyz;
	normal_ = mat3(transpose(inverse(u_ModelMatrix))) * normal;
	viewSpaceNormal_ = (u_ViewMatrix * vec4(normal_, 0.0)).xyz;
	viewPositionViewSpace = (u_ViewMatrix * vec4(positionWorldSpace, 1.0)).xyz;
}
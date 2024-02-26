#version 420

layout(location = 0) in vec3 position;

uniform mat4 modelViewProjMat;

void main() {
	gl_Position = modelViewProjMat * vec4(position, 1.0f);
}
#version 420

uniform vec3 colorIn;

out vec4 FragColor;

void main() {
	FragColor.rgb = colorIn;
}
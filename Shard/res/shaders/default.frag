#version 420

in vec3 normal_;
in vec2 texCoord_;
uniform vec3 colorIn;

out vec4 FragColor;

void main() {
	FragColor.rgb = colorIn;
}
#version 420

precision highp float;

layout(location = 0) out vec4 fragmentColor;

void main() {
	fragmentColor = vec4(gl_FragCoord.z);
}
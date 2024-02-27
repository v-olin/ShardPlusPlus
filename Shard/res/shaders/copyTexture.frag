#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;
layout(binding = 7) uniform sampler2D image;
in vec2 texCoord;

void main()
{
	fragmentColor = texture(image, texCoord);
}

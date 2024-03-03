#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 17) uniform sampler2D overlay;
layout(binding = 18) uniform sampler2D background;

in vec2 texCoord;

uniform float planeHeadingRad = 0.0f;

#define PI 3.14159265359

vec2 rotateVec2(vec2 toRotate, float angle) {
	vec2 p = vec2(toRotate.x, toRotate.y);
	vec2 o = vec2(0.5f, 0.5f);

	float s = sin(angle);
	float c = cos(angle);

	p.x -= o.x;
	p.y -= o.y;

	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	p.x = xnew + o.x;
	p.y = ynew + o.y;
	return p;
}

void main()
{
	vec2 middle = vec2(0.5f, 0.5f);
	float d = distance(middle, texCoord);

	vec4 final = vec4(1.f, 0.0f, 0.0f, 1.0f);
	vec4 overlayColor = texture2D(overlay, texCoord.xy);

	if (overlayColor.a > 0.9f) {
		final = texture2D(overlay, texCoord.xy);
	} else {
		vec2 newCoords = rotateVec2(texCoord.xy, planeHeadingRad);
		final = texture2D(background, newCoords.xy);
	}

	fragmentColor = final;
}

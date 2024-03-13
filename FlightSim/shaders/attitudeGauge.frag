#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 15) uniform sampler2D overlay;
layout(binding = 16) uniform sampler2D background;

in vec2 texCoord;

uniform float planePitchRad;
uniform float planeRollRad;

#define PI 3.14159265359
#define MAGICAL_VALUE 0.38888888f

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

vec2 adjustTexCoords() {
	// background texture aspect ratio is 800:3600
	float aspectRatio = 800.0f / 3600.f;

	// this gives correct roll
	vec2 rollAdjusted = rotateVec2(texCoord, planeRollRad);

	vec2 aspectAdjusted = vec2(rollAdjusted.x, rollAdjusted.y * aspectRatio);

	vec2 pitchAdjusted = vec2(aspectAdjusted.x, aspectAdjusted.y);

	// now we have to add the offset from 0 degrees of pitch
	// there are 200px between each 10s of degrees
	if (planePitchRad != 0.f) {
		if (planePitchRad > 0.f) {
			// a pitch of 90 degrees -> uv.v = 1.f
			// 0 deg pitch -> 0 rad
			// 90 deg pitch -> pi/2 rads
			float rads = planePitchRad;
			// now the pitch is normalized to [0, 1]
			float scaledPitch = 2.0f * rads / PI;
			// now a pitch of 90 deg should equal 1.0f at the center
			float pitchOffset = scaledPitch * 0.5f;
			pitchAdjusted.y += pitchOffset;
		} else {
			// a pitch of -90 deg -> uv.v = 0.f
			float rads = -planePitchRad;
			float scaledPitch = 2.0f * rads / PI;
			float pitchOffset = scaledPitch * 0.5f;
			pitchAdjusted.y -= pitchOffset;
		}
	}

	pitchAdjusted.y += MAGICAL_VALUE;

	return pitchAdjusted;
}

void main()
{
	//fragmentColor = vec4(1.0, 0.0, 1.0, 1.0);
	vec4 overlayColor = texture2D(overlay, texCoord.xy);

	vec2 adjusted = adjustTexCoords();
	vec4 backgroundColor = texture2D(background, adjusted);
	
	// what is your problem green
	vec4 final = vec4(0.0, 1.0, 0.0, 1.0); 

	if (overlayColor.a == 0) {
		final = backgroundColor;
	} else {
		final = overlayColor;
	}

	fragmentColor = final;
}

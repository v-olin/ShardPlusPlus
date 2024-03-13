#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvPosition;

uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float currentTime;
uniform float meshWidth;

out vec3 colorPosition;
out vec2 uvCoords;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;

float hash(float n) { return fract(sin(n) * 753.5453123); }

float noise(vec3 x)
{
	vec3 p = floor(x);
        vec3 f = fract(x);
        f = f * f * (3.0 - 2.0 * f);
        
        float n = p.x + p.y * 157.0 + 113.0 * p.z;
        return mix(mix(mix(hash(n +   0.0), hash(n +   1.0), f.x),
                       mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
                   mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
                       mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

float fbm(vec3 pp)
{
    float f = 0.0;
    mat3 m = mat3(0.00,  0.80,  0.60,
                 -0.80,  0.36, -0.48,
                 -0.60, -0.48,  0.64) * 2;
    f  = 0.5000 * noise(pp); pp = m*pp;
    f += 0.2500 * noise(pp); pp = m*pp;
    f += 0.1250 * noise(pp); pp = m*pp;
    f += 0.0625 * noise(pp); pp = m*pp;
    f += 0.03125 * noise(pp); pp = m*pp;
    f += 0.0150625 * noise(pp); pp = m*pp;
    return f;
}

float waterHeight(vec2 uv)
{
    uv *= 500.0;

    //wave directions (not normalized)
    vec3 waveDir1 = vec3(1.0 / 1.5, 0.0, 1.0 / 8.0);
    vec2 waveDir2 = vec2(1.0 / 5.33, 0.911 / 5.33);
    vec2 waveDir3 = vec2(-1.0 / 1.79, 0.0);

    float f = 1.8 * fbm(currentTime * waveDir1 + vec3(uv / 4.0, 0)) - 0.5;
    float s0 = 0.5 * sin(currentTime * 1.7 + dot(waveDir2, uv));
    float s1 = 0.8 * sin(currentTime * 2.3 + dot(waveDir3, uv) + 3.3 );
    return (s0 + s1 + f) * 0.1;
}

vec3 calcNormal()
{

    //  	A	
	//  B	x	C
	//  	D

    // this is in model space
    
    float ay = waterHeight(vec2(uvPosition.x, uvPosition.y + meshWidth));
    float by = waterHeight(vec2(uvPosition.x - meshWidth, uvPosition.y));
    float cy = waterHeight(vec2(uvPosition.x + meshWidth, uvPosition.y));
    float dy = waterHeight(vec2(uvPosition.x, uvPosition.y - meshWidth));

    vec3 a = vec3(uvPosition.x, ay, uvPosition.y - meshWidth);
    vec3 b = vec3(uvPosition.x - meshWidth, by, uvPosition.y);
    vec3 c = vec3(uvPosition.x + meshWidth, cy, uvPosition.y);
    vec3 d = vec3(uvPosition.x, dy, uvPosition.y + meshWidth);

    vec3 normal = cross(c, a) + cross(a, b) + cross(b, d) + cross(d, c);

    return normalize(-normal);
}

void main()
{
	colorPosition = position;
    vec3 newPos = position;
    newPos.y = waterHeight(uvPosition) * 2 - 20.f;
	gl_Position = modelViewProjectionMatrix * vec4(newPos, 1.0);
    vec3 normal = calcNormal();

	viewSpaceNormal = (normalMatrix * vec4(normal, 0.0)).xyz;
    viewSpacePosition = (modelViewMatrix * vec4(newPos, 1)).xyz;
}
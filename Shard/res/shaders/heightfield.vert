#version 420

///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout(binding = 0) uniform sampler2D heightMap;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoordIn;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////

uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

#define LOADED 0
#define GENERATED 1
uniform int mapType;

uniform uint noiseSeed;
uniform float heightMapSize;
uniform float heightScale;

///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 colorPosition;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;

float getHeight(vec2 pos)
{
    float h = length(texture2D(heightMap, pos).xyz)  * 40 - 20;
    return h;
}

vec3 calcNormalFromTexture(vec2 pos)
{
    float x = texCoordIn.x, y = texCoordIn.y;
    float texOffset = 1.0f / 99;
    float meshOffset = heightMapSize / 99;
    
    //  	A	
    //  B	p	C
    //  	D
    vec2 atex = vec2(texCoordIn.x, texCoordIn.y + texOffset);
    vec2 btex = vec2(texCoordIn.x - texOffset, texCoordIn.y);
    vec2 ctex = vec2(texCoordIn.x + texOffset, texCoordIn.y);
    vec2 dtex = vec2(texCoordIn.x, texCoordIn.y - texOffset);

    vec3 p = position;
    p.y = getHeight(texCoordIn);
    vec3 av = vec3(p.x, getHeight(atex), p.y + meshOffset) - p;
    vec3 bv = vec3(p.x - meshOffset, getHeight(btex), p.y) - p;
    vec3 cv = vec3(p.x + meshOffset, getHeight(ctex), p.y) - p;
    vec3 dv = vec3(p.x, getHeight(dtex), p.y - meshOffset) - p;


    vec3 normal = normalize(
        cross(av, bv) + cross(bv, dv) + cross(dv, cv) + cross(cv, av)
    );

    return normal;
}

void main()
{
	texCoord = texCoordIn;
	vec3 pos = position;
    vec3 norm = normal;

	if (mapType == LOADED) {
        vec4 textureColor = texture2D(heightMap, texCoordIn.xy);
		pos.y = length(textureColor.xyz) * 40 - 20;

        norm = normalize(vec3(0.0f, 1.0f, 0.0f));
        norm = calcNormalFromTexture(texCoord);
    }

	colorPosition = pos;

	gl_Position = modelViewProjectionMatrix * vec4(pos, 1.0);
	viewSpaceNormal = (normalMatrix * vec4(norm, 0.0)).xyz;
	viewSpacePosition = (modelViewMatrix * vec4(pos, 1)).xyz;
}

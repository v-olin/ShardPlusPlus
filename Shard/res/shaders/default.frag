#version 420

precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 0) uniform sampler2D colorMap;
layout(binding = 5) uniform sampler2D emissiveMap;
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
layout(binding = 9) uniform sampler2D shadowMap;

uniform vec3 matColor;
uniform float matMetalness;
uniform float matFresnel;
uniform float matShininess;
uniform vec3 matEmission;

uniform int hasColorTexture = 0;
uniform int hasEmissionTexture = 0;

uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;

uniform float environmentMultiplier = 1.0f;
uniform vec3 sunColor = vec3(1.0f, 1.0f, 1.0f);
uniform float sunIntensityMultiplier = 50.0f;

#define PI 3.14159265359

in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in vec4 shadowMapCoord;

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 direct_illum = base_color;
	
	float d = distance(viewSpaceLightPosition, viewSpacePosition);

	vec3 Li = sunIntensityMultiplier * sunColor * (1/(d*d));

	vec3 wi = normalize(viewSpaceLightPosition - viewSpacePosition);
	
	float dotp = dot(n,wi);

	if (dotp <= 0)
		return vec3(0.0f);

	vec3 diffuse_term = base_color * 1/PI * abs(dotp) * Li;

	vec3 wh = normalize(wi + wo); // can be too short
	float ndotwh = max(0.0001f, dot(n,wh));
	float ndotwo = max(0.0001f, dot(n,wo));
	float wodotwh = max(0.0001f, dot(wo,wh));
	float ndotwi = max(0.0001f, dot(n,wi));
	float whdotwi = max(0.0001f, dot(wh,wi));

	float F = matFresnel + (1 - matFresnel)*pow(1-whdotwi,5);
	float D = ((matShininess + 2)/(2*PI)) * pow(ndotwh,matShininess);

	float left = 2*(ndotwh*ndotwo)/wodotwh;
	float right = 2*(ndotwh*ndotwi)/wodotwh;
	float G = min(1, min(left, right));

	float brdf = (F*D*G)/(4*ndotwo*ndotwi);

	vec3 dielectric_term = brdf * ndotwi * Li + (1-F) * diffuse_term;

	vec3 metal_term = brdf * base_color * dot(n, wi) * Li;

	direct_illum = matMetalness * metal_term + (1-matMetalness) * dielectric_term;

	return direct_illum;
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 indirect_illum = vec3(0.f);
	
	// Calculate the world-space position of this fragment on the near plane
	vec3 world_normal = vec3(viewInverse * vec4(n, 0.0));


	// Calculate the spherical coordinates of the direction
	float theta = acos(max(-1.0f, min(1.0f, world_normal.y)));
	float phi = atan(world_normal.z, world_normal.x);
	if(phi < 0.0f)
	{
		phi = phi + 2.0f * PI;
	}

	// Use these to lookup the color in the environment map
	vec2 lookup = vec2(phi / (2.0 * PI), 1 - theta / PI);

	vec3 diffuse_term = base_color * (1/PI) * environmentMultiplier * texture(irradianceMap, lookup).rgb;

	indirect_illum = diffuse_term;

	vec3 wi = normalize(reflect(-wo, n));
	vec3 wiWorld = normalize(vec3(viewInverse * vec4(wi, 0.0)));
	theta = acos(max(-1.0f, min(1.0f, wiWorld.y)));
	phi = atan(wiWorld.z, wiWorld.x);
	if(phi < 0.0f)
		phi = phi + 2.0f * PI;
	lookup = vec2(phi / (2.0 * PI), 1 - theta / PI);

	float rough = sqrt(sqrt(2/(matShininess+2)));

	vec3 Li = environmentMultiplier * textureLod(reflectionMap, lookup, rough * 7.0).rgb; 

	
	vec3 wh = normalize(wi + wo);
	float wodotwh = max(0.0001f, dot(wo,wh));

	float F = matFresnel + (1 - matFresnel)*pow(1-wodotwh,5);

	vec3 dielectric_term = F * Li + (1-F) * diffuse_term;
	vec3 metal_term = F * base_color * Li;
	vec3 microfacet_term = matMetalness * metal_term + (1 - matMetalness) * dielectric_term;
	
	indirect_illum = microfacet_term;
	return indirect_illum;
}


void main()
{
	float visibility = 1.0f;
	float attenuation = 1.0f;

	float depth = texture(shadowMap, shadowMapCoord.xy / shadowMapCoord.w).r;
	visibility = (depth >= (shadowMapCoord.z / shadowMapCoord.w)) ? 1.0 : 0.0;

	// fuck spotlights

	vec3 wo = -normalize(viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);


	vec3 base_color = matColor;
	if(hasColorTexture == 1) {
		base_color = texture(colorMap, texCoord).rgb;
	}

	vec3 direct_illumination_term = visibility * calculateDirectIllumiunation(wo, n, base_color);

	vec3 indirect_illumination_term = calculateIndirectIllumination(wo, n, base_color);

	vec3 emission_term = matEmission;
	if (hasEmissionTexture == 1)
	{
		emission_term = texture(emissiveMap, texCoord).rgb;
	}

	vec3 final_color = direct_illumination_term + indirect_illumination_term + emission_term + vec3(0.4f) * base_color;

	// Check if we got invalid results in the operations
	if(any(isnan(final_color)))
	{
		final_color.rgb = vec3(1.f, 0.f, 1.f);
	}

	fragmentColor = vec4(final_color, 1.f);
}

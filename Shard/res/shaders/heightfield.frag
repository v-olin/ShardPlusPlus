#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

layout(location = 0) out vec4 fragmentColor;

layout(binding = 1) uniform sampler2D diffTexture;
layout(binding = 2) uniform sampler2D shinyTexture;
layout(binding = 3) uniform sampler2D baseColorMap;
layout(binding = 4) uniform sampler2D normalMap;
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;

in vec2 texCoord;
in vec3 colorPosition;
in vec3 viewSpacePosition;
in vec3 viewSpaceNormal;

uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;

#define LOADED 0
#define GENERATED 1
uniform int mapType;

uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 1000.0;
uniform float environment_multiplier = 1.0f;


#define PI 3.14159265359

// This simple fragment shader is meant to be used for debug purposes
// When the geometry is ok, we will migrate to use shading.frag instead.


vec3 calculateDirectIllumiunation(vec3 wo, vec3 n, vec3 base_color)
{
	vec3 direct_illum = base_color;
	
	float d = distance(viewSpaceLightPosition, viewSpacePosition);

	vec3 Li = point_light_intensity_multiplier * point_light_color * (1/(d*d));

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

	float F = material_fresnel + (1 - material_fresnel)*pow(1-whdotwi,5);
	float D = ((material_shininess + 2)/(2*PI)) * pow(ndotwh,material_shininess);

	float left = 2*(ndotwh*ndotwo)/wodotwh;
	float right = 2*(ndotwh*ndotwi)/wodotwh;
	float G = min(1, min(left, right));

	float brdf = (F*D*G)/(4*ndotwo*ndotwi);

	vec3 dielectric_term = brdf * ndotwi * Li + (1-F) * diffuse_term;

	vec3 metal_term = brdf * base_color * dot(n, wi) * Li;

	direct_illum = material_metalness * metal_term + (1-material_metalness) * dielectric_term;

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
	if(phi < 0.0f) {
		phi = phi + 2.0f * PI;
	}

	// Use these to lookup the color in the environment map
	vec2 lookup = vec2(phi / (2.0 * PI), 1 - theta / PI);

	vec3 diffuse_term = base_color * (1/PI) * environment_multiplier * texture(irradianceMap, lookup).rgb;

	indirect_illum = diffuse_term;

	vec3 wi = normalize(reflect(-wo, n));
	vec3 wiWorld = normalize(vec3(viewInverse * vec4(wi, 0.0)));
	theta = acos(max(-1.0f, min(1.0f, wiWorld.y)));
	phi = atan(wiWorld.z, wiWorld.x);
	if(phi < 0.0f)
		phi = phi + 2.0f * PI;
	lookup = vec2(phi / (2.0 * PI), 1 - theta / PI);

	float rough = sqrt(sqrt(2/(material_shininess+2)));

	vec3 Li = environment_multiplier * textureLod(reflectionMap, lookup, rough * 7.0).rgb; 

	
	vec3 wh = normalize(wi + wo);
	float wodotwh = max(0.0001f, dot(wo,wh));

	float F = material_fresnel + (1 - material_fresnel)*pow(1-wodotwh,5);

	vec3 dielectric_term = F * Li + (1-F) * diffuse_term;
	vec3 metal_term = F * base_color * Li;
	vec3 microfacet_term = material_metalness * metal_term + (1-material_metalness) * dielectric_term;
	
	indirect_illum = microfacet_term;
	return indirect_illum;
}

void main() {
	vec3 texColor = vec3(1.0f, 0, 0);

	if (mapType == GENERATED) {
		texColor = (texture2D(baseColorMap, (texCoord.xy * 150))).rgb;
	}
	else {
		texColor = (texture2D(diffTexture, texCoord.xy)).rgb;
	}

	vec3 wo = -normalize(viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	vec3 final = calculateDirectIllumiunation(wo, n, texColor);
	vec3 indirect = calculateIndirectIllumination(wo, n, texColor);

	float shininess = texture2D(shinyTexture, texCoord.xy).r;

	final += indirect * shininess;
	final += 0.5 * texColor;

	if (any(isnan(final)))
	{
		final.rgb = vec3(1.f, 0.f, 0.f);
	}

	fragmentColor.rgb = final;
}

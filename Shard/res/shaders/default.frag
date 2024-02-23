#version 420

in vec3 normal_;
in vec2 texCoord_;

in vec3 positionWorldSpace;
in vec3 viewPositionWorldSpace;

///////////
// Light //
///////////
uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;
uniform float u_LightAmbientIntensity; // not used atm. so it is optimized away, don't try and set it
uniform float u_LightDiffuseIntensity;
uniform float u_LightDpecularIntensity;

// Light attenuation properties
uniform float u_AttenuationConstant = 1.0;
uniform float u_AttenuationLinear = 0.001;
uniform float u_AttenuationQuadratic = 0.002;

uniform vec3 u_ObjectColor;

layout (binding = 9) uniform sampler2D colorTex;
layout (binding = 6) uniform sampler2D reflectionMap;
uniform bool hasColTex;
uniform float environment_multiplier = 1.0;
uniform mat4 viewInverse;
#define PI 3.14159265359

out vec4 FragColor;

vec3 CalculateColor() {

	vec3 lightVec = u_LightPosition - positionWorldSpace;

	// Ambient
	vec3 ambient = u_LightAmbientIntensity * u_LightColor;

	// Diffuse
	vec3 norm = normalize(normal_);
	vec3 lightDir = normalize(lightVec);
	vec3 diffuse = max(dot(norm, lightDir), 0.0) * u_LightDiffuseIntensity * u_LightColor;

	// (Phong) Specular
    vec3 viewDir = normalize(viewPositionWorldSpace - positionWorldSpace);
    vec3 reflectDir = reflect(-lightDir, norm);  
	vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 16) * u_LightDpecularIntensity * u_LightColor;

	// Apply attenuation
//	float dist = length(lightVec);
//	float attenuation = 1.0 / (u_AttenuationConstant + u_AttenuationLinear * dist + u_AttenuationQuadratic * dist * dist);
//	ambient *= attenuation;
//	diffuse *= attenuation;
//	specular *= attenuation;

	// Sample reflectance map

	vec3 wo = -normalize(viewPositionWorldSpace);
	vec3 n = normalize(normal_);
	vec3 wi = (viewInverse * vec4(reflect(-wo, n), 0.0)).xyz;

	float theta2 = acos(max(-1.0f, min(1.0f, wi.y)));
	float phi2 = atan(wi.z, wi.x);
	if(phi2 < 0.0f)
		phi2 = phi2 + 2.0f * PI;

	// ------------------------------v use material_shininess property in future
	float roughness = sqrt(sqrt(2 / (0 + 2)));
	vec2 lookup2 = vec2(phi2 / (2.0 * PI), 1 - theta2 / PI);
	vec3 Li2 = environment_multiplier * texture(reflectionMap, lookup2, roughness * 7.0).rgb;

	///////////////////////////////////////////////////////////////////////////

	vec3 color = u_ObjectColor;
	if(hasColTex)
		color = texture(colorTex, texCoord_.xy).rgb;
//		color = vec3(1.0, 0, 1.0);
	vec3 final = color * Li2 * (ambient + diffuse + specular);
//	vec3 final = u_ObjectColor * Li2 * (ambient + diffuse + specular);

	return final;

}

void main() {
	vec3 PointLightColor = CalculateColor();
	FragColor = vec4(PointLightColor, 1.0);
}
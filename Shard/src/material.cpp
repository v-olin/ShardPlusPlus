#include "material.h"
//#include "labhelper.h"
#include "sampling.h"
#include <cmath>

//using namespace labhelper;

namespace Shard {
namespace PathTracer {
///////////////////////////////////////////////////////////////////////////
// Generate a vector that is perpendicular to another
///////////////////////////////////////////////////////////////////////////
glm::vec3 perpendicular(const glm::vec3& v)
{
	if (fabsf(v.x) < fabsf(v.y))
	{
		return glm::vec3(0.0f, -v.z, v.y);
	}
	return glm::vec3(-v.z, 0.0f, v.x);
}

///////////////////////////////////////////////////////////////////////////
// Creates a TBN matrix for the tangent space orthonormal to N
// We use the method from Duff et al. "Building an Orthonormal Basis, Revisited"
// https://jcgt.org/published/0006/01/01/
// which uses quaternion math to calculate the tangent vectors
///////////////////////////////////////////////////////////////////////////
glm::mat3 tangentSpace(glm::vec3 n)
{
	float sign = copysignf(1.0f, n.z);
	const float a = -1.0f / (sign + n.z);
	const float b = n.x * n.y * a;
	glm::mat3 r;
	r[0] = glm::vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
	r[1] = glm::vec3(b, sign + n.y * n.y * a, -n.y);
	r[2] = n;
	return r;
}


	//vec3 or_black(vec3 v) noexcept {
	//	if (std::isnan(v.x) || std::isnan(v.x) || std::isnan(v.x)) {
	//		return vec3(0);
	//	}
	//	return v;
	//}

	WiSample sampleHemisphereCosine(const vec3& wo, const vec3& n) {
		mat3 tbn = tangentSpace(n);
		vec3 sample = cosineSampleHemisphere();
		WiSample r;
		r.wi = tbn * sample;
		if (dot(r.wi, n) > 0.0f) {
			r.pdf = max(0.0f, dot(r.wi, n)) / M_PI;
		}
		return r;
	}

	///////////////////////////////////////////////////////////////////////////
	// A Lambertian (diffuse) material
	///////////////////////////////////////////////////////////////////////////
	vec3 Diffuse::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		if (dot(wi, n) <= 0.0f) {
			return vec3(0.0f);
		}
		if (!sameHemisphere(wi, wo, n)) {
			return vec3(0.0f);
		}
		return (1.0f / M_PI) * color;
	}

	WiSample Diffuse::sample_wi(const vec3& wo, const vec3& n) const {
		WiSample r = sampleHemisphereCosine(wo, n);
		r.f = f(r.wi, wo, n);
		return r;
	}

	vec3 MicrofacetBRDF::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		vec3 wh = glm::normalize(wi + wo);

		float s = shininess;
		float d = (s + 2) / (2 * M_PI) * pow(dot(n, wh), s);
		float nwo = dot(n, wh) * dot(n, wo) / dot(wo, wh);
		float nwi = dot(n, wh) * dot(n, wi) / dot(wo, wh);
		float g = min(1.0f, min(nwo, nwi) * 2.0f);

		float x = (d * g) / (4 * dot(n, wo) * dot(n, wi));
		return vec3(x);
	}
	WiSample MicrofacetBRDF::sample_wi(const vec3& wo, const vec3& n) const {

		WiSample r;
		float s = shininess;
		vec3 tangent = normalize(perpendicular(n));
		vec3 bitangent = normalize(cross(tangent, n));
		float phi = 2.0f * M_PI * randf();
		float cos_theta = pow(randf(), 1.0f / (s + 1));
		float sin_theta = sqrt(max(0.0f, 1.0f - cos_theta * cos_theta));
		vec3 wh = normalize(
			sin_theta * cos(phi) * tangent + sin_theta * sin(phi) * bitangent
			+ cos_theta * n
		);
		vec3 wi = -reflect(wo, wh);

		float pdf_wh = (s + 1.0f) * pow(dot(n, wh), s) * (1.0f / (2.0f * M_PI));
		float pdf_wi = pdf_wh / (4.0f * dot(wo, wh));
		r.pdf = pdf_wi;
		r.wi = wi;
		r.f = f(r.wi, wo, n);
		return r;
	}

	float BSDF::fresnel(const vec3& wi, const vec3& wo) const {
		vec3 wh = normalize(wi + wo);
		return (float)(R0 + (1.0 - R0) * pow(1.0 - dot(wh, wi), 5));
	}

	vec3 DielectricBSDF::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		vec3 BRDF = reflective_material->f(wi, wo, n);
		vec3 BTDF = transmissive_material->f(wi, wo, n);
		float F = fresnel(wi, wo);

		return F * BRDF + (1.0f - F) * BTDF;
	}

	WiSample DielectricBSDF::sample_wi(const vec3& wo, const vec3& n) const {
		WiSample r{};
		float fo = 0;

		if (dot(n, wo) >= 0.0f) {
			fo = (R0 + (1.0f - R0) * pow(1.0f - abs(dot(n, wo)), 5));
		}

		if (randf() < fo) {
			// Sample the BRDF
			r = reflective_material->sample_wi(wo, n);
			r.pdf *= fo;
			vec3 wh = normalize(r.wi + wo);
			r.f *= (R0 + (1.0f - R0) * pow(1.0f - abs(dot(wo, wh)), 5));
		}
		else {
			// Sample the BTDF
			r = transmissive_material->sample_wi(wo, n);
			r.pdf *= 1.0f - fo;
			r.f *= 1.0f - fo;
		}

		return r;
	}

	vec3 MetalBSDF::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		vec3 BRDF = reflective_material->f(wi, wo, n);
		float F = fresnel(wi, wo);

		return F * BRDF * color;
	}

	WiSample MetalBSDF::sample_wi(const vec3& wo, const vec3& n) const {

		WiSample r;
		{
			// Sample the BRDF
			r = reflective_material->sample_wi(wo, n);
			r.f *= fresnel(r.wi, wo) * this->color;
		}
		return r;
	}

	vec3 BSDFLinearBlend::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		auto lhs = this->bsdf0->f(wi, wo, n);
		auto rhs = this->bsdf1->f(wi, wo, n);

		return lhs * w + rhs * (1.0f - w);
	}

	WiSample BSDFLinearBlend::sample_wi(const vec3& wo, const vec3& n) const {
		if (randf() < w) {
			return this->bsdf0->sample_wi(wo, n);
		}
		return this->bsdf1->sample_wi(wo, n);
	}

#if SOLUTION_PROJECT == PROJECT_REFRACTIONS
	///////////////////////////////////////////////////////////////////////////
	// A perfect specular refraction.
	///////////////////////////////////////////////////////////////////////////
	vec3 GlassBTDF::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		if (sameHemisphere(wi, wo, n)) {
			return vec3(0);
		}
		else {
			return vec3(1);
		}
	}

	WiSample GlassBTDF::sample_wi(const vec3& wo, const vec3& n) const {
		WiSample r;

		float eta;
		glm::vec3 N;
		if (dot(wo, n) > 0.0f) {
			N = n;
			eta = 1.0f / ior;
		}
		else {
			N = -n;
			eta = ior;
		}

		// Alternatively:
		// d = dot(wo, N)
		// k = d * d (1 - eta*eta)
		// wi = normalize(-eta * wo + (d * eta - sqrt(k)) * N)

		// or

		// d = dot(n, wo)
		// k = 1 - eta*eta * (1 - d * d)
		// wi = - eta * wo + ( eta * d - sqrt(k) ) * N

		float w = dot(wo, N) * eta;
		float k = 1.0f + (w - eta) * (w + eta);
		if (k < 0.0f) {
			// Total internal reflection
			r.wi = reflect(-wo, n);
		}
		else {
			k = sqrt(k);
			r.wi = normalize(-eta * wo + (w - k) * N);
		}
		r.pdf = abs(dot(r.wi, n));
		r.f = vec3(1.0f, 1.0f, 1.0f);

		return r;
	}

	vec3 BTDFLinearBlend::f(const vec3& wi, const vec3& wo, const vec3& n) const {
		return w * btdf0->f(wi, wo, n) + (1.0f - w) * btdf1->f(wi, wo, n);
	}

	WiSample BTDFLinearBlend::sample_wi(const vec3& wo, const vec3& n) const {
		if (randf() < w) {
			return btdf0->sample_wi(wo, n);
		}
		else {
			return btdf1->sample_wi(wo, n);
		}
	}

#endif
} // namespace PathTracer
} // namespace Shard

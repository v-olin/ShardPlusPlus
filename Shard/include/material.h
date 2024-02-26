#pragma once
#include <glm.hpp>
#include "Pathtracer.h"
#include "sampling.h"

using namespace glm;

namespace Shard
{
namespace PathTracer
{
struct WiSample
{
	vec3 wi = vec3(0);
	vec3 f = vec3(0);
	float pdf = 0.f;
};

///////////////////////////////////////////////////////////////////////////
/// Interface for Bidirectional Reflection Functions
///////////////////////////////////////////////////////////////////////////
class BRDF
{
public:
	// Return the value of the brdf for specific directions
	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const = 0;
	// Sample a suitable direction and return the brdf in that direction as
	// well as the pdf (~probability) that the direction was chosen.
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const = 0;
};

///////////////////////////////////////////////////////////////////////////
/// Interface for Bidirectional Transmission Functions
///////////////////////////////////////////////////////////////////////////
class BTDF
{
public:
	// Return the value of the btdf for specific directions
	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const = 0;

	// Sample a suitable direction and return the btdf in that direction as
	// well as the pdf (~probability) that the direction was chosen.
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const = 0;
};


///////////////////////////////////////////////////////////////////////////
/// Bidirectional Scattering Function
///////////////////////////////////////////////////////////////////////////
class BSDF
{
public:
	float R0 = 0.f;

	BSDF() = default;
	BSDF(float _Fresnel_R0) : R0(_Fresnel_R0)
	{
	}

	// Return the value of the bsdf for specific directions
	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const = 0;

	// Sample a suitable direction and return the bsdf in that direction as
	// well as the pdf (~probability) that the direction was chosen.
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const = 0;

	// Calculate the fresnel term
	float fresnel(const vec3& wi, const vec3& wo) const;
};


///////////////////////////////////////////////////////////////////////////
/// A Lambertian (diffuse) material
///////////////////////////////////////////////////////////////////////////
class Diffuse : public BTDF
{
public:
	vec3 color;
	Diffuse(vec3 c) : color(c)
	{
	}
	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};


///////////////////////////////////////////////////////////////////////////
/// A Microfacet BRFD
///////////////////////////////////////////////////////////////////////////
class MicrofacetBRDF : public BRDF
{
public:
	float shininess;
	MicrofacetBRDF(float _shininess) : shininess(_shininess)
	{
	}
	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};


///////////////////////////////////////////////////////////////////////////
/// A Dielectric BSFD
///////////////////////////////////////////////////////////////////////////
class DielectricBSDF : public BSDF
{
public:
	const BRDF* reflective_material = nullptr;
	const BTDF* transmissive_material = nullptr;

	DielectricBSDF(const BRDF* reflective, const BTDF* refractive, float _Fresnel_R0)
	    : BSDF(_Fresnel_R0), reflective_material(reflective), transmissive_material(refractive)
	{
	}

	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};

///////////////////////////////////////////////////////////////////////////
/// A Metal BSFD
///////////////////////////////////////////////////////////////////////////
class MetalBSDF : public BSDF
{
public:
	vec3 color;
	const BRDF* reflective_material = nullptr;

	MetalBSDF(const BRDF* reflective, vec3 _color, float _Fresnel_R0)
	    : BSDF(_Fresnel_R0), reflective_material(reflective), color(_color)
	{
	}

	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/// A Linear Blend between two BRDFs
///////////////////////////////////////////////////////////////////////////
class BSDFLinearBlend : public BSDF
{
public:
	float w;
	BSDF* bsdf0;
	BSDF* bsdf1;

	BSDFLinearBlend(float _w, BSDF* a, BSDF* b) : w(_w), bsdf0(a), bsdf1(b)
	{
	}

	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;

	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};

#if SOLUTION_PROJECT == PROJECT_REFRACTIONS
///////////////////////////////////////////////////////////////////////////
// A perfect specular refraction.
///////////////////////////////////////////////////////////////////////////
class GlassBTDF : public BTDF
{
public:
	float ior;

	GlassBTDF(float _ior) : ior(_ior)
	{
	}

	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;
	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};

class BTDFLinearBlend : public BTDF
{
public:
	float w;
	BTDF* btdf0;
	BTDF* btdf1;

	BTDFLinearBlend(float _w, BTDF* a, BTDF* b) : w(_w), btdf0(a), btdf1(b)
	{
	}

	virtual vec3 f(const vec3& wi, const vec3& wo, const vec3& n) const override;

	virtual WiSample sample_wi(const vec3& wo, const vec3& n) const override;
};
#endif


} // namespace PathTracer
} // namespace Shard

#pragma once
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include "Model.h"
#include <glm.hpp>
#include <map>

namespace Shard
{
namespace PathTracer
{
///////////////////////////////////////////////////////////////////////////
// This struct describes an intersection, as extracted from the Embree
// ray.
///////////////////////////////////////////////////////////////////////////
struct Intersection
{
	// Point where the ray intersected with geometry
	glm::vec3 position;

	// Normal of the intersected triangle
	glm::vec3 geometry_normal;

	// Interpolated normal between the three vertex normals of the triangle
	glm::vec3 shading_normal;

	// "outgoing" vector. Pointing from the intersected point to the origin of the ray.
	glm::vec3 wo;

	// Interpolated UV coordinates between the 3 vertices of the triangle
	glm::vec2 uv;

	// Material information of the hit triangle
	const Shard::Material* material;
};

///////////////////////////////////////////////////////////////////////////
// This struct is what an embree Ray must look like. It contains the
// information about the ray to be shot and (after intersect() has been
// called) the geometry the ray hit.
///////////////////////////////////////////////////////////////////////////
struct RTCORE_ALIGN(16) Ray
{
	Ray(const glm::vec3& origin = glm::vec3(0.0f),
	    const glm::vec3& direction = glm::vec3(0.0f),
	    float near = 0.0f,
	    float far = FLT_MAX)
	    : o(origin), d(direction), tnear(near), tfar(far)
	{
		geomID = RTC_INVALID_GEOMETRY_ID;
		primID = RTC_INVALID_GEOMETRY_ID;
		instID = RTC_INVALID_GEOMETRY_ID;
	}

	////////////////////////////
	// Ray data

	// `o`: origin position of the ray
	glm::vec3 o;
	float align0;

	// `d`: direction of the ray
	glm::vec3 d;
	float align1;

	// `tnear`, `tfar`: starting distance and final distance to look for intersections
	float tnear = 0.0f, tfar = FLT_MAX;
	float time = 0.0f;
	uint32_t mask = 0xFFFFFFFF;


	////////////////////////////
	// Hit Data (do not modify)

	// Normal of the surface
	glm::vec3 n;
	float align2;

	// UV of the intersected triangle
	float u, v;

	uint32_t geomID = RTC_INVALID_GEOMETRY_ID;
	uint32_t primID = RTC_INVALID_GEOMETRY_ID;
	uint32_t instID = RTC_INVALID_GEOMETRY_ID;
};

///////////////////////////////////////////////////////////////////////////
// Scene functions
///////////////////////////////////////////////////////////////////////////

// Add a model to the embree scene
void addModel(const Shard::Model* model, const glm::mat4& model_matrix);

// Build an acceleration structure for the scene
void buildBVH();

///////////////////////////////////////////////////////////////////////////
// Reinitialize the scene
///////////////////////////////////////////////////////////////////////////
void reinitScene();


///////////////////////////////////////////////////////////////////////////
// Ray intersection functions
///////////////////////////////////////////////////////////////////////////

// Test a ray against the scene and find the closest intersection
bool intersect(Ray& r);

// This returns the intersection information for a ray.
// Use after calling `intersect`
Intersection getIntersection(const Ray& r);


// Test whether a ray is intersected anywhere by the scene
// (does not return an intersection, as it doesn't find the closest one)
bool occluded(Ray& r);

} // namespace PathTracer
} // namespace pathtracer

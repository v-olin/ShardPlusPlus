#include "sampling.h"
#include <random>
#include <omp.h>
#include <iostream>
#include <glm.hpp>

using namespace glm;

#define M_PI 3.14159265359f
namespace Shard
{
	namespace PathTracer
	{

std::mt19937 generators[24]; // Assuming no more than 24 cores
float randf()
{
	return float(generators[omp_get_thread_num()]() / double(generators[omp_get_thread_num()].max()));
}

glm::vec2 concentricSampleDisk()
{
	float r, theta;
	float u1 = randf();
	float u2 = randf();
	float sx = 2 * u1 - 1;
	float sy = 2 * u2 - 1;
	if(sx == 0.0 && sy == 0.0)
	{
		return glm::vec2(0, 0);
	}
	if(sx >= -sy)
	{
		if(sx > sy)
		{ // Handle first region of disk
			r = sx;
			if(sy > 0.0)
				theta = sy / r;
			else
				theta = 8.0f + sy / r;
		}
		else
		{ // Handle second region of disk
			r = sy;
			theta = 2.0f - sx / r;
		}
	}
	else
	{
		if(sx <= sy)
		{ // Handle third region of disk
			r = -sx;
			theta = 4.0f - sy / r;
		}
		else
		{ // Handle fourth region of disk
			r = -sy;
			theta = 6.0f + sx / r;
		}
	}
	theta *= float(M_PI) / 4.0f;
	return r * glm::vec2(cosf(theta), sinf(theta));
}

glm::vec3 cosineSampleHemisphere()
{
	glm::vec3 ret(concentricSampleDisk(), 0);
	ret.z = sqrt(max(0.f, 1.f - ret.x * ret.x - ret.y * ret.y));
	return ret;
}

bool sameHemisphere(const vec3& i, const vec3& o, const vec3& n)
{
	return sign(dot(o, n)) == sign(dot(i, n));
}
} // namespace Pathtracer
} // namespace Shard
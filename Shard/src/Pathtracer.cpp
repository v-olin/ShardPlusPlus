#include "Pathtracer.h"
#include "embree.h"
//#include "labhelper.h"
#include "material.h"
#include "sampling.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>

using namespace std;
using namespace glm;
//using namespace labhelper;

namespace Shard {
namespace PathTracer {
	///////////////////////////////////////////////////////////////////////////////
	// Global variables
	///////////////////////////////////////////////////////////////////////////////
	Settings settings;
	Environment environment;
	Image rendered_image;
	PointLight point_light;
	std::vector<DiscLight> disc_lights;

	///////////////////////////////////////////////////////////////////////////
	// Restart rendering of image
	///////////////////////////////////////////////////////////////////////////
	void restart() {
		// No need to clear image,
		rendered_image.number_of_samples = 0;
	}

	int getSampleCount() {
		return std::max(rendered_image.number_of_samples - 1, 0);
	}

	///////////////////////////////////////////////////////////////////////////
	// On window resize, window size is passed in, actual size of pathtraced
	// image may be smaller (if we're subsampling for speed)
	///////////////////////////////////////////////////////////////////////////
	void resize(int w, int h) {
		rendered_image.width = w / settings.subsampling;
		rendered_image.height = h / settings.subsampling;
		rendered_image.data.resize(rendered_image.width * rendered_image.height);
		restart();
	}

	///////////////////////////////////////////////////////////////////////////
	/// Return the radiance from a certain direction wi from the environment
	/// map.
	///////////////////////////////////////////////////////////////////////////
	vec3 Lenvironment(const vec3& wi) {
		const float theta = acos(std::max(-1.0f, std::min(1.0f, wi.y)));
		float phi = atan(wi.z, wi.x);
		if (phi < 0.0f) {
			phi = phi + 2.0f * M_PI;
		}
		vec2 lookup = vec2(phi / (2.0 * M_PI), 1 - theta / M_PI);
		return environment.multiplier * environment.map.sample(lookup.x, lookup.y);
	}

	vec3 Li(Ray& primary_ray) {
		vec3 L = vec3(0.0, 0.0, 0.0);
		vec3 path_throughput = vec3(1, 1, 1);
		Ray current_ray = primary_ray;

		for (size_t bounces = 0; bounces < settings.max_bounces; ++bounces) {
			Intersection hit = getIntersection(current_ray);

			Diffuse diffuse(hit.material->m_color);
			GlassBTDF glass(hit.material->m_ior);
			BTDFLinearBlend glass_blend(
				hit.material->m_transparency, &glass, &diffuse
			);
			MicrofacetBRDF microfacet(hit.material->m_shininess);
			DielectricBSDF dielectric(
				&microfacet, &glass_blend, hit.material->m_fresnel
			);
			MetalBSDF metal(
				&microfacet, hit.material->m_color, hit.material->m_fresnel
			);
			BSDFLinearBlend metal_blend(
				hit.material->m_metalness, &metal, &dielectric
			);
			auto& mat = metal_blend;

			vec3 direct_illumination = vec3(0.0, 0.0, 0.0);
			{
				vec3 wi_unnorm = point_light.position - hit.position;
				vec3 wi = normalize(wi_unnorm);

				Ray light_ray(hit.position + hit.shading_normal * EPSILON, wi);

				if (!occluded(light_ray)) {
					// const float falloff_factor =
					// 	1.0f / dot(wi_unnorm, wi_unnorm);
					const float distance_to_light =
						length(point_light.position - hit.position);
					const float falloff_factor =
						1.0f / (distance_to_light * distance_to_light);

					vec3 Li = point_light.intensity_multiplier * point_light.color
						* falloff_factor;
					direct_illumination =
						mat.f(wi, hit.wo, hit.shading_normal) * Li
						* std::max(0.0f, dot(wi, hit.shading_normal));
				}
			}
			vec3 emission = vec3(0.0, 0.0, 0.0);
			{
				emission = hit.material->m_emission;
			}

			L += path_throughput * (direct_illumination + emission);
			WiSample sample = mat.sample_wi(hit.wo, hit.shading_normal);
			if (sample.pdf < EPSILON) {
				return L;
			}
			float cosineterm = abs(dot(sample.wi, hit.shading_normal));
			path_throughput *= (sample.f * cosineterm) / sample.pdf;
			if (path_throughput == vec3(0.0, 0.0, 0.0)) {
				return L;
			}

			current_ray = Ray(hit.position, sample.wi);

			if (dot(hit.shading_normal, sample.wi) > 0.0) {
				current_ray.o += 0.0001f * hit.shading_normal;
			}
			else {
				current_ray.o -= 0.0001f * hit.shading_normal;
			}

			if (!intersect(current_ray)) {
				return L + path_throughput * Lenvironment(current_ray.d);
			}

			// path_throughput *= (f * cosineterm) / pdf
		}
		return L;
	}

	///////////////////////////////////////////////////////////////////////////
	/// Used to homogenize points transformed with projection matrices
	///////////////////////////////////////////////////////////////////////////
	inline static glm::vec3 homogenize(const glm::vec4& p) {
		return glm::vec3(p * (1.f / p.w));
	}

	///////////////////////////////////////////////////////////////////////////
	/// Trace one path per pixel and accumulate the result in an image
	///////////////////////////////////////////////////////////////////////////
	void tracePaths(const glm::mat4& V, const glm::mat4& P) {
		// Stop here if we have as many samples as we want
		if ((int(rendered_image.number_of_samples) > settings.max_paths_per_pixel)
			&& (settings.max_paths_per_pixel != 0))
		{
			return;
		}
		vec3 camera_pos = vec3(glm::inverse(V) * vec4(0.0f, 0.0f, 0.0f, 1.0f));
		// Trace one path per pixel (the omp parallel stuf magically distributes the
		// pathtracing on all cores of your CPU).
		int num_rays = 0;
		vector<vec4> local_image(
			rendered_image.width * rendered_image.height, vec4(0.0f)
		);

		// TODO: parallel Z-curve/loop blocking, more efficient multisampling
#pragma omp parallel for
		for (int y = 0; y < rendered_image.height; y++) {
			for (int x = 0; x < rendered_image.width; x++) {
				vec3 colour = vec3(0); // Black
				Ray primaryRay;
				primaryRay.o = camera_pos;
				// Create a ray that starts in the camera position and points toward
				// the current pixel on a virtual screen.
				vec2 screenCoord = vec2(
					(randf() + float(x)) / float(rendered_image.width),
					(randf() + float(y)) / float(rendered_image.height)
				);
				// Calculate direction
				vec4 viewCoord = vec4(
					screenCoord.x * 2.0f - 1.0f,
					screenCoord.y * 2.0f - 1.0f,
					1.0f,
					1.0f
				);
				vec3 p = homogenize(inverse(P * V) * viewCoord);
				primaryRay.d = normalize(p - camera_pos);
				// Intersect ray with scene
				if (intersect(primaryRay)) {
					// If it hit something, evaluate the radiance from that point
					colour = Li(primaryRay);

				}
				else {
					// Otherwise evaluate environment
					colour = Lenvironment(primaryRay.d);
				}

				// Accumulate the obtained radiance to the pixels color
				float n = float(rendered_image.number_of_samples);
				vec3& px = rendered_image.data[y * rendered_image.width + x];
				vec3 next = px * (n / (n + 1.0f)) + (1.0f / (n + 1.0f)) * colour;

				if (!(std::isnan(next.x) || std::isnan(next.y)
					|| std::isnan(colour.z)))
				{
					px = next;
				}
			}
		}
		rendered_image.number_of_samples += 1;
	}
} // namespace PathTracer
} // namespace Shard

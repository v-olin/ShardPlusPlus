#pragma once

#include <glm.hpp>

class Noise
{
public:
	static float perlin(float x, float y, float size, int seed);
	static float perlin(float x, float y, float size, int seed, int octaves);

private:
	static glm::vec2 randGrad(int ix, int iy, int seed);
	static float dotGridGrad(int ix, int iy, float x, float y, int seed);
	static float interp(float a, float b, float w);
};
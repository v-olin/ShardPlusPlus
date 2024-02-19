 #include "noise.h"
#include <glm.hpp>
#include <math.h>

// Explanation: https://www.youtube.com/watch?v=kCIaHqb60Cw

float Noise::perlin(float x, float y, float size, int seed, int octaves)
{
    float noise = 0, freq = 1.0f, amp = 1.0f;

    for (int i = 0; i < octaves; i++)
    {
        noise += amp * perlin(
            x * freq / size,
            y * freq / size,
            size, seed);
        freq *= 2;
        amp /= 2;
    }

    noise *= 1.2f; // contrast

    if (noise > 1.0f) noise = 1.0f;
    else if (noise < -1.0f) noise = -1.0f;

    return noise;
}

float Noise::perlin(float x, float y, float size, int seed)
{
    // shift to positive coordinates
    // noise is fcked with negative coords
    x = x + (size / 2); 
    y = y + (size / 2);

    // corner coords
    int x0 = int(x), y0 = int(y);
    int x1 = x0 + 1, y1 = y0 + 1;

    float sx = x - float(x0);
    float sy = y - float(y0);

    // interpolate top two corners
    float n0 = dotGridGrad(x0, y0, x, y, seed);
    float n1 = dotGridGrad(x1, y0, x, y, seed);
    float ix = interp(n0, n1, sx);

    // interpolate bottom two corners
    n0 = dotGridGrad(x0, y1, x, y, seed);
    n1 = dotGridGrad(x1, y1, x, y, seed);
    float yx = interp(n0, n1, sx);

    // interpolate in 2nd dimension
    return interp(ix, yx, sy);
}

// hashing function, stolen from the inter-webz
glm::vec2 Noise::randGrad(int ix, int iy, int seed)
{
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= seed;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    return glm::vec2(sin(random), cos(random));
}

float Noise::dotGridGrad(int ix, int iy, float x, float y, int seed)
{
    glm::vec2 gradient = randGrad(ix, iy, seed);

    float dx = x - float(ix);
    float dy = y - float(iy);

    return dx * gradient.x + dy * gradient.y;
}

float Noise::interp(float a, float b, float w)
{
    // quinitc interpolation
    return (b - a) * (3.0f - w * 2.0f) * w * w + a;
}
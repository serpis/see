#ifndef _HEIGHTMAP_HPP
#define _HEIGHTMAP_HPP

#include "math.hpp"

struct heightmap_t
{
    int width, height;
    const float *heights;
    vec3_t<> xyz_low;
    vec3_t<> xyz_high;
};

extern heightmap_t create_heightmap(int width, int height, vec3_t<> xyz_low, vec3_t<> xyz_high);
extern float sample_heightmap(const heightmap_t &heightmap, float x, float z);

#endif // _HEIGHTMAP_HPP


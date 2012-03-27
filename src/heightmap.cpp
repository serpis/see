#include <cmath>

#include "noise.hpp"
#include "heightmap.hpp"

#define BLEND(a, b, t) ((1.0f - t) * a + t * b)


template <class T>
typename T::component_type rnoise(int n, T p)
{
    if (n == 1)
    {
        return noise(p);
    }
    else
    {
        return noise(p) + typename T::component_type(0.5) * rnoise(n - 1, typename T::component_type(2.0) * p);
    }
}

vec2_t<> perturb(int width, int height, vec2_t<> p)
{
    vec2_t<> res;
    res.x = p.x + 0.01f * width * noise(0.02f * p);
    res.y = p.y + 0.01f * height * noise(0.02f * p + vec2_t<>(1.5, 6.7));
    return res;
}

float sample_noise(vec2_t<> p)
{
    return 0.5f * rnoise(9, 0.003f * p);
}

float sample_voronoi(int width, int height, vec2_t<> p)
{
    const int xs = 5;
    const int ys = 5;
    vec2_t<> points[xs * ys];
    for (int y = 0; y < ys; y++)
    {
        for (int x = 0; x < xs; x++)
        {
            int index = x + y * xs;
            points[index].x = width * (x + 0.5f) / xs;
            points[index].y = height * (y + 0.5f) / ys;

            points[index].x += width / xs * noise(vec3_t<>(x, y, 0));
            points[index].y += height / ys * noise(vec3_t<>(x, y, 100));
        }
    }

    int closest_index = 0;
    for (int i = 1; i < xs * ys; i++)
    {
        if ((points[i] - p).length() < (points[closest_index] - p).length())
        {
            closest_index = i;
        }
    }

    int next_closest_index = xs * ys - 1 - closest_index;
    for (int i = 0; i < xs * ys; i++)
    {
        if (i != closest_index && (points[i] - p).length() < (points[next_closest_index] - p).length())
        {
            next_closest_index = i;
        }
    }

    return 0.007f * (-(points[closest_index] - p).length() + (points[next_closest_index] - p).length()) - 0.5f;
}


void blur_map(int width, int height, float *to, float *from)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int dxs[4] = { -1, 0, 1, 0 };
            int dys[4] = { 0, -1, 0, 1 };

            float accum = 0.0f;

            for (int i = 0; i < 4; i++)
            {
                int sample_x = x + dxs[i];
                int sample_y = y + dys[i];

                if (sample_x >= 0 && sample_x < width && sample_y >= 0 && sample_y < height)
                {
                    int sample_index = sample_x + sample_y * width;
                    accum += from[sample_index];
                }
            }

            int to_index = x + y * width;
            to[to_index] = 0.25f * accum;
        }
    }
}

void erode(int width, int height, float *buffer)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int to_index = x + y * width;
            float h = buffer[to_index];

            float threshold = 12.0f / width;

            int dxs[4] = { -1, 0, 1, 0 };
            int dys[4] = { 0, -1, 0, 1 };

            float dmax = 0.0f;
            int max_index;

            for (int i = 0; i < 4; i++)
            {
                int sample_x = x + dxs[i];
                int sample_y = y + dys[i];

                if (sample_x >= 0 && sample_x < width && sample_y >= 0 && sample_y < height)
                {
                    int sample_index = sample_x + sample_y * width;
                    float di = buffer[to_index] - buffer[sample_index];
                    if (di > dmax)
                    {
                        dmax = di;
                        max_index = sample_index;
                    }
                }
            }

            if (dmax > 0.0f && dmax <= threshold)
            {
                float dh = 0.5f * dmax;
                buffer[to_index] -= dh;
                buffer[max_index] += dh;
            }
        }
    }
}

void generate_heights(float *map, int width, int height)
{
    /*for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = x + y * width;
            map[index] = 0.41f;
        }
    }
    return;*/

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = x + y * width;
            vec2_t<> pos(x, y);
            pos = perturb(width, height, pos);
            map[index] = 0.5f * (sample_voronoi(width, height, pos) + sample_noise(pos)) + 0.5f;
        }
    }

    for (int i = 0; i < 50; i++)
    {
        erode(width, height, map);
    }
}


void subdivide_map(int width, int height, int subdivisions, float *to, float *from)
{
    int to_width = width * subdivisions;
    int to_height = height * subdivisions;

    for (int y = 0; y < to_height; y++)
    {
        for (int x = 0; x < to_width; x++)
        {
            int index = x + y * to_width;

            float from_x = ((float)x) / subdivisions;
            float from_y = ((float)y) / subdivisions;

            int from_x0 = (int)from_x;
            int from_x1 = from_x0 + 1;

            float blend_x = from_x - from_x0;

            int from_y0 = (int)from_y;
            int from_y1 = from_y0 + 1;

            float blend_y = from_y - from_y0;

            int index00 = from_x0 + from_y0 * width;
            int index01 = from_x1 + from_y0 * width;
            int index10 = from_x0 + from_y1 * width;
            int index11 = from_x1 + from_y1 * width;

            float a = BLEND(from[index00], from[index01], blend_x);
            float b = BLEND(from[index10], from[index11], blend_x);

            to[index] = BLEND(a, b, blend_y);
        }
    }
}

float sample_heightmap(const heightmap_t &heightmap, float x, float z)
{
    if (x < heightmap.xyz_low.x)
    {
        x = heightmap.xyz_low.x;
    }
    else if (x > heightmap.xyz_high.x)
    {
        x = heightmap.xyz_high.x;
    }
    if (z < heightmap.xyz_low.z)
    {
        z = heightmap.xyz_low.z;
    }
    else if (z > heightmap.xyz_high.z)
    {
        z = heightmap.xyz_high.z;
    }

    vec3_t<> diff = heightmap.xyz_high - heightmap.xyz_low;
    
    float lookup_x = heightmap.width * (x - heightmap.xyz_low.x) / diff.x;
    float lookup_y = heightmap.height * (z - heightmap.xyz_low.z) / diff.z;

    int lx0 = (int)floorf(lookup_x);
    int lx1 = lx0 + 1;
    int ly0 = (int)floorf(lookup_y);
    int ly1 = ly0 + 1;

    float frac_x = lookup_x - floorf(lookup_x);
    float frac_y = lookup_y - floorf(lookup_y);

    float height00 = heightmap.heights[lx0 + ly0 * heightmap.width];
    float height01 = heightmap.heights[lx1 + ly0 * heightmap.width];
    float height10 = heightmap.heights[lx0 + ly1 * heightmap.width];
    float height11 = heightmap.heights[lx1 + ly1 * heightmap.width];

    float a = BLEND(height00, height01, frac_x);
    float b = BLEND(height10, height11, frac_x);

    return heightmap.xyz_low.y + diff.y * BLEND(a, b, frac_y);
}

heightmap_t create_heightmap(int width, int height, vec3_t<> xyz_low, vec3_t<> xyz_high)
{
    float *map = new float[width * height];

    generate_heights(map, width, height);

    float *blurred_map = new float[width * height];

    blur_map(width, height, blurred_map, map);
    //blur_map(width, height, map, blurred_map);
    //blur_map(width, height, blurred_map, map);

    float *subdivided_map = new float[width * 4 * height * 4];
    float *blurred_subdivided_map = new float[width * 4 * height * 4];

    subdivide_map(width, height, 4, subdivided_map, map);
    blur_map(width * 4, height * 4, blurred_subdivided_map, subdivided_map);

    heightmap_t res;
    res.width = width * 4;
    res.height = height * 4;
    res.heights = blurred_subdivided_map;
    res.xyz_low = xyz_low;
    res.xyz_high = xyz_high;

    delete[] subdivided_map;
    delete[] blurred_map;
    delete[] map;

    return res;
}


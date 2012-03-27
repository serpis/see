#include <vector>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>

#include <GL/gl.h> // for constants
#include <GL/glfw.h> // so that loading screen can be rendered

#include "engine.hpp"
#include "renderl.hpp"
#include "renderm.hpp"
#include "renderh.hpp"
#include "material.hpp"
#include "item.hpp"
#include "resources.hpp"
#include "math.hpp"
#include "noise.hpp"
#include "components.hpp"
#include "entity_system.hpp"
#include "heightmap.hpp"

extern int window_width;
extern int window_height;
extern float window_aspect;
extern bool running;
extern bool window_fullscreen;

#define BLEND(a, b, t) ((1.0f - t) * a + t * b)

#include <sys/time.h>
double precision_time_now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec + tv.tv_usec / 1000000.0;
};

renderm_mesh_t create_water_mesh(int width, int height)
{
    float hw = ((float)width)/2.0f;
    float hh = ((float)height)/2.0f;

    float positions[] = 
    {
        -hw, 0.0f,  hh,
         hw, 0.0f,  hh,
        -hw, 0.0f, -hh,
         hw, 0.0f, -hh,
    };

    float normals[] =
    {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };

    float tangents[] =
    {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
    };

    unsigned char indices[4];
    for (int i = 0; i < 4; i++)
    {
        indices[i] = i;
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(positions));
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, sizeof(normals));
    res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    res.tex_coord_buffer.handle = 0;
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
    res.index_count = 4;
    res.primitive_type = GL_TRIANGLE_STRIP;

    return res;
}


renderm_mesh_t create_portal_mesh()
{
    float positions[] = 
    {
        0.0f, -0.5f,  0.5f,
        0.0f, -0.5f, -0.5f,
        0.0f,  0.5f,  0.5f,
        0.0f,  0.5f, -0.5f,
    };

    float normals[] =
    {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
    };

    float tangents[] =
    {
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
    };

    float tex_coords[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    unsigned char indices[4];
    for (int i = 0; i < 4; i++)
    {
        indices[i] = i;
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(positions));
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, sizeof(normals));
    res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
    res.index_count = 4;
    res.primitive_type = GL_TRIANGLE_STRIP;

    return res;
}

renderm_mesh_t create_cube_mesh()
{
    float positions[] =
    {
        // front
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

        // right
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,

        // back
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // left
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // top
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // bottom
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
    };

    float normals[] =
    {
        // front
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // right
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // back
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // left
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // top
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // bottom
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
    };

    float tangents[] =
    {
        // front
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // right
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,
         0.0f,  0.0f, -1.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // back
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // left
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // top
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,
         1.0f,  0.0f,  0.0f,

         0.0f,  0.0f,  0.0f,
         0.0f,  0.0f,  0.0f,

        // bottom
         1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
    };

    float tex_coords[] =
    {
        // front
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 0.0f,

        // right
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 0.0f,

        // back
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 0.0f,

        // left
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 0.0f,

        // top
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,

        0.0f, 0.0f,
        0.0f, 0.0f,

        // bottom
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    unsigned char indices[6*4 + 5*2];
    int index_count = sizeof(indices)/sizeof(indices[0]);
    for (int i = 0; i < index_count; i++)
    {
        indices[i] = i;
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(positions));
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, sizeof(normals));
    res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
    res.index_count = index_count;
    res.primitive_type = GL_TRIANGLE_STRIP;

    return res;
}

struct triangle_t
{
    vec3_t<> positions[3];
    vec3_t<> normals[3];
};

vec3_t<> field_normal(float (*field)(vec3_t<> p), vec3_t<> p)
{
    const float d = 0.01f;
    const vec3_t<> dx(d, 0.0f, 0.0f);
    const vec3_t<> dy(0.0f, d, 0.0f);
    const vec3_t<> dz(0.0f, 0.0f, d);

    return vec3_t<>(field(p + dx) - field(p - dx),
                    field(p + dy) - field(p - dy),
                    field(p + dz) - field(p - dz)).normalized();
}

template <class T>
int polygonize(vec3_t<T> points[8], T (*field)(vec3_t<T> p), float isolevel, triangle_t *triangles)
{
    const int edgeTable[256] =
    {
        0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
        0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
        0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
        0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
        0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
        0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
        0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
        0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
        0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
        0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
        0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
        0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
        0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
        0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
        0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
        0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
        0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
        0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
        0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
        0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
        0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
        0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
        0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
        0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
        0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
        0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
        0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
        0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
        0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
        0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
        0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
        0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
    };

    const int triTable[256][16] =
    {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
        {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
        {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
        {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
        {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
        {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
        {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
        {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
        {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
        {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
        {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
        {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
        {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
        {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
        {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
        {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
        {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
        {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
        {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
        {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
        {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
        {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
        {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
        {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
        {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
        {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
        {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
        {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
        {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
        {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
        {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
        {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
        {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
        {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
        {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
        {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
        {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
        {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
        {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
        {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
        {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
        {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
        {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
        {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
        {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
        {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
        {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
        {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
        {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
        {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
        {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
        {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
        {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
        {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
        {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
        {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
        {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
        {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
        {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
        {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
        {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
        {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
        {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
        {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
        {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
        {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
        {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
        {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
        {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
        {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
        {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
        {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
        {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
        {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
        {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
        {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
        {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
        {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
        {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
        {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
        {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
        {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
        {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
        {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
        {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
        {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
        {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
        {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
        {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
        {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
        {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
        {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
        {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
        {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
        {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
        {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
        {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
    };

    //  Determine the index into the edge table which
    //  tells us which vertices are inside of the surface
    int cubeindex = 0;
    for (int i = 0; i < 8; i++)
    {
        if (field(points[i]) < isolevel)
        {
            cubeindex |= (1 << i);
        }
    }

    // Cube is entirely in/out of the surface
    if (edgeTable[cubeindex] == 0)
    {
        return 0;
    }

    vec3_t<T> vertlist[12];
    // Find the vertices where the surface intersects the cube
    for (int i = 0; i < 12; i++)
    {
        if (edgeTable[cubeindex] & (1 << i))
        {
            const int i0[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3 };
            const int i1[12] = { 1, 2, 3, 0, 5, 6, 7, 4, 4, 5, 6, 7 };

            vec3_t<T> inside;
            vec3_t<T> outside;
            if (field(points[i0[i]]) < isolevel)
            {
                inside = points[i0[i]];
                outside = points[i1[i]];
            }
            else
            {
                outside = points[i0[i]];
                inside = points[i1[i]];
            }

            float lower_t = 0.0f;
            float upper_t = 1.0f;
            vec3_t<> p;
            for (int j = 0; j < 6; j++)
            {
                float middle_t = (lower_t + upper_t) / 2.0f;
                p = BLEND(inside, outside, middle_t);
                if (field(p) < isolevel)
                {
                    lower_t = middle_t;
                }
                else
                {
                    upper_t = middle_t;
                }
            }
            //p = BLEND(points[i0//[i]], points[i1[i]], 0.5f);
            vertlist[i] = p;
        }
    }

    // Create the triangles
    int ntriang = 0;
    for (int i = 0; triTable[cubeindex][i] != -1; i += 3)
    {
        for (int j = 0; j < 3; j++)
        {
            triangles[ntriang].positions[j] = vertlist[triTable[cubeindex][i + j]];
            triangles[ntriang].normals[j] = field_normal(field, triangles[ntriang].positions[j]);
        }
        ntriang++;
    }

    return ntriang;
}

struct node_t
{
    bool open;
    bool closed;
    float cost;
    float heur;
    node_t *parent;
    vec3_t<> position;
    float angle;
};

bool node_compare(node_t *a, node_t *b)
{
    return a->cost + a->heur > b->cost + b->heur;
}

int gcd(int a, int b)
{
    if (b == 0)
    {
        return a;
    }
    else
    {
        return gcd(b, a % b);
    }
}

float node_cost(node_t *map, int xsize, int ysize, int zsize, node_t *from, node_t *to)
{
    const int xs[4] = { -1, 0, 1, 0 };
    const int ys[4] = { 0, -1, 0, 1 };

    float dhmax = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        int x = to->position.x + xs[i];
        int y = to->position.y + ys[i];
        if (x >= 0 && x < xsize && y >= 0 && y < ysize)
        {
            float dh = fabs(map[x + y * xsize].position.z - to->position.z);
            dhmax = std::max(dh, dhmax);
        }
    }

    float dz = fabs(to->position.z - from->position.z);

    float angle = atan2f(to->position.y - from->position.y, to->position.x - from->position.x);
    float dangle = from->angle - angle;
    while (dangle >= 3.14159f)
    {
        dangle -= 3.14159f;
    }
    while (dangle <= -3.14159f)
    {
        dangle += 3.14159f;
    }
    dangle = fabs(dangle);

    if (dz > 1)
    {
        return 100000;
    }
    else
    {
        //return 0;
        //return /*10.0f * dhmax * dhmax*/ + (expf(4.0f * dz) - 1);
        return 10.0f * dhmax * dhmax + (expf(40.0f * dz) - 1) + (100.0f * expf(dangle) - 1);
    }
}

#include <queue>
#include <deque>
#include <vector>
#include <algorithm>

template <class T, class Compare>
class my_heap
{
private:
    int element_count;
    T a[50000];
    const Compare &compare;
public:
    my_heap(const Compare &compare) : compare(compare)
    {
        element_count = 0;
    }

    int size()
    {
        return this->element_count;
    }

    bool empty()
    {
        return this->size() == 0;
    }

    void swap_elements(int ia, int ib)
    {
        T t = a[ia];
        a[ia] = a[ib];
        a[ib] = t;
    }

    void push(T x)
    {
        assert(element_count < 50000);
        a[element_count++] = x;

        sift_up(0, element_count - 1);
    }

    T pop()
    {
        T front = a[0];
        a[0] = a[element_count - 1];
        element_count -= 1;

        sift_down(0, element_count - 1);

        return front;
    }

    void sift_up(int start, int end)
    {
        int child = end;
        while (child > start)
        {
            int parent = (child - 1) / 2;
            if (compare(a[parent], a[child]))
            {
                swap_elements(parent, child);
                child = parent;
            }
            else
            {
                break;
            }
        }
    }

    void sift_down(int start, int end)
    {
        int root = start;
        while (root * 2 + 1 <= end)
        {
            int child = root * 2 + 1;
            int swap = root;
            if (compare(a[swap], a[child]))
            {
                swap = child;
            }
            if (child + 1 <= end && compare(a[swap], a[child + 1]))
            {
                swap = child + 1;
            }
            if (swap != root)
            {
                swap_elements(root, swap);
                root = swap;
            }
            else
            {
                break;
            }
        }
    }

    void random_remove(T x)
    {
        for (int i = 0; i < element_count; i++)
        {
            if (a[i] == x)
            {
                swap_elements(i, element_count - 1);
                element_count -= 1;
                sift_down(i, element_count - 1);
                return;
            }
        }
        assert(0);
    }
};

template <class T, class Container, class Compare>
class my_priority_queue
{
private:
    const Compare &compare;
    Container *container;
public:
    my_priority_queue(const Compare &compare_func, Container *container) : compare(compare_func), container(container)
    {
    }

    int size()
    {
        return this->container->size();
    }

    void push(T o)
    {
        this->container->push_back(o);
        std::make_heap(this->container->begin(), this->container->end(), compare);
    }

    void pop()
    {
        this->container->pop_front();
    }

    T top()
    {
        return this->container->front();
    }

    bool empty()
    {
        return this->size() == 0;
    }

};

node_t *find_path(node_t *map, int xsize, int ysize, int zsize, node_t *from, node_t *to)
{
    int neighbor_count = 0;
    float r = 4.0f;
    int neighbor_xs[80];
    int neighbor_ys[80];
    int neighbor_zs[80];
    for (int y = -5; y <= 5; y++)
    {
        for (int x = -5; x <= 5; x++)
        {
            if (gcd(abs(x), abs(y)) == 1 && x*x + y*y <= r*r)
            {
                neighbor_xs[neighbor_count] = x;
                neighbor_ys[neighbor_count] = y;
                neighbor_zs[neighbor_count] = 0;
                neighbor_count += 1;
                printf("%d %d\n", x, y);
            }
        }
    }
    printf("%d\n", neighbor_count);

    //std::deque<node_t *> underlying_vector;
    //my_priority_queue<node_t *, std::deque<node_t *>, bool (*)(node_t *a, node_t *b)> pq(node_compare, &underlying_vector);
    my_heap<node_t *, bool (*)(node_t *, node_t *)> pq(node_compare);
    float dx = from->position.x - to->position.x;
    float dy = from->position.y - to->position.y;
    float heur = fabs(dx) + fabs(dy);
    from->heur = heur;
    pq.push(from);
    int investigated = 0;
    while (!pq.empty())
    {
        //printf("%d\n", (int)pq.size());
        node_t *n = pq.pop();

        investigated += 1;
        //printf("size: %d, investigated: %d\n", pq.size(), investigated);

        assert(!n->closed);

        n->closed = true;

        //printf("(%.4f) %f, %f ?= %f, %f\n", n->cost + n->heur, n->position.x, n->position.y, to->position.x, to->position.y);

        if (n->position.x == to->position.x && n->position.y == to->position.y)
        {
            printf("investigated: %d\n", investigated);
            return n;
        }
        else
        {
            for (int i = 0; i < neighbor_count; i++)
            {
                int x = n->position.x + neighbor_xs[i];
                int y = n->position.y + neighbor_ys[i];
                int z = 0;//n->position.z + neighbor_zs[i];
                if (x >= 0 && x < xsize && y >= 0 && y < ysize && z >= 0 && z < zsize)
                {
                    node_t *neighbor = &map[x + y * xsize + z * xsize * ysize];
                    if (neighbor->closed)
                    {
                        continue;
                    }
                    float cost = n->cost + (neighbor->position - n->position).xy().length() + node_cost(map, xsize, ysize, zsize, n, neighbor);
                    float dx = neighbor->position.x - to->position.x;
                    float dy = neighbor->position.y - to->position.y;
                    float heur = (neighbor->position - to->position).xy().length();
                    if (!neighbor->open)
                    {
                        neighbor->open = true;
                        neighbor->parent = n;
                        neighbor->cost = cost;
                        neighbor->heur = heur;
                        neighbor->angle = atan2f(neighbor->position.y - n->position.y, neighbor->position.x - n->position.x);
                        pq.push(neighbor);
                    }
                    else if (cost + heur < neighbor->cost + neighbor->heur)
                    {
                        pq.random_remove(neighbor);
                        neighbor->parent = n;
                        neighbor->cost = cost;
                        neighbor->heur = heur;
                        neighbor->angle = atan2f(neighbor->position.y - n->position.y, neighbor->position.x - n->position.x);
                        pq.push(neighbor);
                    }
                }
            }
        }
    }


    return NULL;
}

/*renderl_texture_t create_heightmap_texture(int width, int height)
{
    float *map = new float[width * height];
    generate_heights(map, width, height);

    node_t *nodes = new node_t[width * height];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = x + y * width;
            node_t *n = &nodes[index];
            n->open = false;
            n->closed = false;
            n->heur = 0.0f;
            n->cost = 0.0f;
            n->parent = NULL;
            n->position.x = x;
            n->position.y = y;
            n->position.z = map[index] * 5.0f;
            n->angle = 0.0f;
        }
    }

    unsigned char *field = new unsigned char[width * height * 3];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = x + y * width;
            field[index * 3 + 0] = 255 * map[index];
            field[index * 3 + 1] = 255 * map[index];
            field[index * 3 + 2] = 255 * map[index];
        }
    }

    renderl_texture_t res = renderl_upload_texture(width, height, GL_RGB, field);

    delete[] field;
    delete[] map;

    return res;
}*/


renderm_mesh_t create_field_mesh(float (*field)(vec3_t<> p), float isolevel, vec3_t<> xyz_low_corner, vec3_t<> xyz_high_corner, int resolution)
{
    const vec3_t<> diff = xyz_high_corner - xyz_low_corner;
    const int cells = resolution * resolution * resolution;
    const int max_tris_per_cell = 5;
    int total_triangle_count = 0;
    triangle_t *triangles = new triangle_t[cells * max_tris_per_cell];
    for (int i = 0; i < resolution; i++)
    {
        for (int j = 0; j < resolution; j++)
        {
            for (int k = 0; k < resolution; k++)
            {
                float x0 = xyz_low_corner.x + (i + 0) * diff.x / resolution;
                float x1 = xyz_low_corner.x + (i + 1) * diff.x / resolution;
                float y0 = xyz_low_corner.y + (j + 0) * diff.y / resolution;
                float y1 = xyz_low_corner.y + (j + 1) * diff.y / resolution;
                float z0 = xyz_low_corner.z + (k + 0) * diff.z / resolution;
                float z1 = xyz_low_corner.z + (k + 1) * diff.z / resolution;

                vec3_t<> points[8] = 
                {
                    vec3_t<>(x0, y0, z0),
                    vec3_t<>(x1, y0, z0),
                    vec3_t<>(x1, y0, z1),
                    vec3_t<>(x0, y0, z1),
                    vec3_t<>(x0, y1, z0),
                    vec3_t<>(x1, y1, z0),
                    vec3_t<>(x1, y1, z1),
                    vec3_t<>(x0, y1, z1),
                };
                total_triangle_count += polygonize(points, field, isolevel, &triangles[total_triangle_count]);
            }
        }
    }

    vec3_t<> *positions = new vec3_t<>[3 * total_triangle_count];
    vec3_t<> *normals = new vec3_t<>[3 * total_triangle_count];
    unsigned int *indices = new unsigned int[3 * total_triangle_count];

    for (int i = 0; i < total_triangle_count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            positions[i * 3 + j] = triangles[i].positions[j];
            normals[i * 3 + j] = triangles[i].normals[j];
            indices[i * 3 + j] = i * 3 + j;
        }
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, 3 * 3 * sizeof(float) * total_triangle_count);
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, 3 * 3 * sizeof(float) * total_triangle_count);
    res.tangent_buffer.handle = 0;
    res.tex_coord_buffer.handle = 0;
    //res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    //res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_INT, indices, 3 * sizeof(unsigned int) * total_triangle_count);
    res.index_count = 3 * total_triangle_count;
    res.primitive_type = GL_TRIANGLES;

    delete[] triangles;
    delete[] positions;
    delete[] indices;
    delete[] normals;

    return res;
}

renderm_mesh_t create_heightmap_mesh(const heightmap_t &heightmap)
{
    vec3_t<> diff = heightmap.xyz_high - heightmap.xyz_low;

    vec3_t<> *points = new vec3_t<>[heightmap.width * heightmap.height];
    vec3_t<> *normals = new vec3_t<>[heightmap.width * heightmap.height];
    int index_count = 2 * 3 * (heightmap.width - 1) * (heightmap.height - 1);
    unsigned int *indices = new unsigned int[index_count];
    int ip = 0;

    // set all points and reset normals
    for (int y = 0; y < heightmap.height; y++)
    {
        for (int x = 0; x < heightmap.width; x++)
        {
            int index = x + y * heightmap.width;
            points[index].x = heightmap.xyz_low.x + (((float)x) / (heightmap.width - 1)) * diff.x;
            points[index].y = heightmap.xyz_low.y + heightmap.heights[index] * diff.y;
            points[index].z = heightmap.xyz_low.z + (((float)y) / (heightmap.height - 1)) * diff.z;

            normals[index] = vec3_t<>(0.0f, 0.0f, 0.0f);
        }
    }

    // loop through all cells (quadratic areas)
    for (int col = 0; col < heightmap.height - 1; col++)
    {
        for (int row = 0; row < heightmap.width - 1; row++)
        {
            // each such cell has two triangles...
            int index_a, index_b, index_c;
            vec3_t<> *a, *b, *c;
            vec3_t<> *na, *nb, *nc;
            vec3_t<> ab, ac;
            vec3_t<> normal;


            // select the point/normal indices for the first triangle
            index_a = row + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + col * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac).normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;

            indices[ip++] = index_a;
            indices[ip++] = index_b;
            indices[ip++] = index_c;


            // select the point/normal indices for the second triangle
            index_a = (row + 1) + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + (col + 1) * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac).normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;

            indices[ip++] = index_a;
            indices[ip++] = index_b;
            indices[ip++] = index_c;
        }
    }

    assert(ip == index_count);

    // normalize all normals
    for (int i = 0; i < heightmap.width * heightmap.height; i++)
    {
        points[i].y = 0.0f;
        normals[i] = normals[i].normalized();
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, points, 3 * sizeof(float) * heightmap.width * heightmap.height);
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, 3 * sizeof(float) * heightmap.width * heightmap.height);
    res.tangent_buffer.handle = 0;
    res.tex_coord_buffer.handle = 0;
    //res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    //res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_INT, indices, sizeof(unsigned int) * index_count);
    res.index_count = index_count;
    res.primitive_type = GL_TRIANGLES;

    delete[] points;
    delete[] normals;

    return res;
}

renderm_mesh_t create_heightmap_mesh2(const heightmap_t &heightmap)
{
    float rwidth = 0.03f;

    int radius_count = 1000;
    int segment_count = 32;

    int index_count = radius_count * segment_count * 6;

    vec3_t<> *positions = new vec3_t<>[radius_count * segment_count * 6];
    vec3_t<> *normals = new vec3_t<>[radius_count * segment_count * 6];
    vec3_t<> *p = positions;
    vec3_t<> *n = normals;

    unsigned short *indices = new unsigned short[index_count];

    for (int r = 0; r < radius_count; r++)
    {
        for (int s = 0; s < segment_count; s++)
        {
            float radians0 = 2.0f * M_PI * s / (float)segment_count;
            float radians1 = 2.0f * M_PI * (s + 1) / (float)segment_count;

            float radius0 = pow(rwidth * r, 2.0f);
            float radius1 = pow(rwidth * (r + 1), 2.0f);

            float x0 = radius0 * cos(radians0);
            float x1 = radius1 * cos(radians0);
            float x2 = radius0 * cos(radians1);
            float x3 = x2;
            float x4 = x1;
            float x5 = radius1 * cos(radians1);

            float z0 = radius0 * sin(-radians0);
            float z1 = radius1 * sin(-radians0);
            float z2 = radius0 * sin(-radians1);
            float z3 = z2;
            float z4 = z1;
            float z5 = radius1 * sin(-radians1);

            *p++ = vec3_t<>(x0, 0.0f, z0);
            *p++ = vec3_t<>(x1, 0.0f, z1);
            *p++ = vec3_t<>(x2, 0.0f, z2);
            *p++ = vec3_t<>(x3, 0.0f, z3);
            *p++ = vec3_t<>(x4, 0.0f, z4);
            *p++ = vec3_t<>(x5, 0.0f, z5);

            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
            *n++ = vec3_t<>(0.0f, 1.0f, 0.0f);
        }
    }

    for (int i = 0; i < index_count; i++)
    {
        indices[i] = i;
    }

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(vec3_t<>) * radius_count * segment_count * 6);
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, sizeof(vec3_t<>) * radius_count * segment_count * 6);
    res.tangent_buffer.handle = 0;
    res.tex_coord_buffer.handle = 0;
    //res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    //res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_SHORT, indices, sizeof(unsigned int) * index_count);
    res.index_count = index_count;
    res.primitive_type = GL_TRIANGLES;

    delete[] positions;
    delete[] normals;
    delete[] indices;

    return res;
}

renderm_mesh_t create_heightmap_mesh3(const heightmap_t &heightmap)
{
    vec3_t<> diff = heightmap.xyz_high - heightmap.xyz_low;

    vec3_t<> *points = new vec3_t<>[heightmap.width * heightmap.height];
    vec3_t<> *normals = new vec3_t<>[heightmap.width * heightmap.height];
    int index_count = 2 * 3 * (heightmap.width - 1) * (heightmap.height - 1);
    unsigned int *indices = new unsigned int[index_count];
    int ip = 0;

    // set all points and reset normals
    for (int y = 0; y < heightmap.height; y++)
    {
        for (int x = 0; x < heightmap.width; x++)
        {
            int index = x + y * heightmap.width;
            points[index].x = heightmap.xyz_low.x + (((float)x) / (heightmap.width - 1)) * diff.x;
            points[index].y = heightmap.xyz_low.y + heightmap.heights[index] * diff.y;
            points[index].z = heightmap.xyz_low.z + (((float)y) / (heightmap.height - 1)) * diff.z;

            normals[index] = vec3_t<>(0.0f, 0.0f, 0.0f);
        }
    }

    // loop through all cells (quadratic areas)
    for (int col = 0; col < heightmap.height - 1; col++)
    {
        for (int row = 0; row < heightmap.width - 1; row++)
        {
            // each such cell has two triangles...
            int index_a, index_b, index_c;
            vec3_t<> *a, *b, *c;
            vec3_t<> *na, *nb, *nc;
            vec3_t<> ab, ac;
            vec3_t<> normal;


            // select the point/normal indices for the first triangle
            index_a = row + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + col * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac);//.normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;

            indices[ip++] = index_a;
            indices[ip++] = index_b;
            indices[ip++] = index_c;


            // select the point/normal indices for the second triangle
            index_a = (row + 1) + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + (col + 1) * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac);//.normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;

            indices[ip++] = index_a;
            indices[ip++] = index_b;
            indices[ip++] = index_c;
        }
    }

    assert(ip == index_count);

    // normalize all normals
    for (int i = 0; i < heightmap.width * heightmap.height; i++)
    {
        points[i].y = 0.0f;
        normals[i] = normals[i].normalized();
    }

    int middle_x = heightmap.width / 2;
    int middle_y = heightmap.height / 2;

    int sector_width = heightmap.width / 16;
    int sector_height = heightmap.height / 16;

    ip = 0;

    for (int sec_y = 0; sec_y < 16; sec_y++)
    {
        for (int sec_x = 0; sec_x < 16; sec_x++)
        {
            int sec_middle_x = sector_width * sec_x + 64;
            int sec_middle_y = sector_height * sec_y + 64;

            int bdx = sec_middle_x - middle_x;
            int bdy = sec_middle_y - middle_y;

            float distance = sqrt(bdx * bdx + bdy * bdy);

            int d = (int)(0.008 * distance);
            if (d > 10)
            {
                d = 10;
            }
            int gran = (int)(powf(2, d) + 0.5f);
            if (gran == 0)
            {
                gran = 1;
            }
            if (gran > 16)
            {
                gran = 16;
            }
            //int gran = 16;

            for (int dy = 0; dy < 128; dy += gran)
            {
                for (int dx = 0; dx < 128; dx += gran)
                {
                    int x0 = sec_x * sector_width + dx;
                    int x1 = sec_x * sector_width + dx + gran;
                    int y0 = sec_y * sector_height + dy;
                    int y1 = sec_y * sector_height + dy + gran;

                    if (x1 >= heightmap.width || y1 >= heightmap.height)
                    {
                        continue;
                    }

                    int index_a = x0 + y0 * heightmap.width;
                    int index_b = x0 + y1 * heightmap.width;
                    int index_c = x1 + y0 * heightmap.width;

                    indices[ip++] = index_a;
                    indices[ip++] = index_b;
                    indices[ip++] = index_c;

                    index_a = x1 + y0 * heightmap.width;
                    index_b = x0 + y1 * heightmap.width;
                    index_c = x1 + y1 * heightmap.width;

                    indices[ip++] = index_a;
                    indices[ip++] = index_b;
                    indices[ip++] = index_c;
                }
            }
        }
    }

    index_count = ip;

    renderm_mesh_t res;
    res.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, points, 3 * sizeof(float) * heightmap.width * heightmap.height);
    res.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, 3 * sizeof(float) * heightmap.width * heightmap.height);
    res.tangent_buffer.handle = 0;
    res.tex_coord_buffer.handle = 0;
    //res.tangent_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, tangents, sizeof(tangents));
    //res.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
    res.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_INT, indices, sizeof(unsigned int) * index_count);
    res.index_count = index_count;
    res.primitive_type = GL_TRIANGLES;

    delete[] points;
    delete[] normals;
    delete[] indices;

    return res;
}

renderl_texture_t create_heightmap_normal_map(const heightmap_t &heightmap)
{
    vec3_t<> diff = heightmap.xyz_high - heightmap.xyz_low;

    vec3_t<> *points = new vec3_t<>[heightmap.width * heightmap.height];
    vec3_t<> *normals = new vec3_t<>[heightmap.width * heightmap.height];
    int ip = 0;

    // set all points and reset normals
    for (int y = 0; y < heightmap.height; y++)
    {
        for (int x = 0; x < heightmap.width; x++)
        {
            int index = x + y * heightmap.width;
            points[index].x = heightmap.xyz_low.x + (((float)x) / (heightmap.width - 1)) * diff.x;
            points[index].y = heightmap.xyz_low.y + heightmap.heights[index] * diff.y;
            points[index].z = heightmap.xyz_low.z + (((float)y) / (heightmap.height - 1)) * diff.z;

            normals[index] = vec3_t<>(0.0f, 0.0f, 0.0f);
        }
    }

    // loop through all cells (quadratic areas)
    for (int col = 0; col < heightmap.height - 1; col++)
    {
        for (int row = 0; row < heightmap.width - 1; row++)
        {
            // each such cell has two triangles...
            int index_a, index_b, index_c;
            vec3_t<> *a, *b, *c;
            vec3_t<> *na, *nb, *nc;
            vec3_t<> ab, ac;
            vec3_t<> normal;


            // select the point/normal indices for the first triangle
            index_a = row + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + col * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac).normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;


            // select the point/normal indices for the second triangle
            index_a = (row + 1) + col * heightmap.width;
            index_b = row + (col + 1) * heightmap.width;
            index_c = (row + 1) + (col + 1) * heightmap.width;

            a = &points[index_a];
            b = &points[index_b];
            c = &points[index_c];

            na = &normals[index_a];
            nb = &normals[index_b];
            nc = &normals[index_c];

            ab = *b - *a;
            ac = *c - *a;
            normal = ab.cross(ac).normalized();

            *na += normal;
            *nb += normal;
            *nc += normal;
        }
    }

    // normalize all normals
    for (int i = 0; i < heightmap.width * heightmap.height; i++)
    {
        points[i].y = 0.0f;
        normals[i] = normals[i].normalized();
    }

    unsigned char *data = new unsigned char[3 * heightmap.width * heightmap.height];
    for (int i = 0; i < heightmap.width * heightmap.height; i++)
    {
        vec3_t<> n = normals[i];
        data[i * 3 + 0] = (unsigned char)(255.0f * (0.5f * n.x + 0.5f));
        data[i * 3 + 1] = (unsigned char)(255.0f * (0.5f * n.y + 0.5f));
        data[i * 3 + 2] = (unsigned char)(255.0f * (0.5f * n.z + 0.5f));
    }

    renderl_texture_t res = renderl_upload_texture(heightmap.width, heightmap.height, GL_RGB, data);

    delete[] data;
    delete[] points;
    delete[] normals;

    return res;
}

renderl_texture_t create_heightmap_texture(const heightmap_t &heightmap)
{
    float *data = new float[heightmap.width * heightmap.height * 3];

    for (int y = 0; y < heightmap.height; y++)
    {
        for (int x = 0; x < heightmap.width; x++)
        {
            int index = x + y * heightmap.width;
            data[index * 3 + 0] = heightmap.heights[index];
            data[index * 3 + 1] = heightmap.heights[index];
            data[index * 3 + 2] = heightmap.heights[index];
        }
    }

    renderl_texture_t res = renderl_upload_texture_adv(heightmap.width, heightmap.height, GL_RGBA16F, GL_RGB, GL_FLOAT, data);

    delete[] data;

    return res;
}

/*float field(vec3_t p)
{
    return p.length();
}*/


template <class T>
T field(vec3_t<T> p)
{
    //return p.y + 0.5f * rnoise(5, p);
    //T v = p.y - T(0.5) * rnoise(4, T(0.2) * p.xz()) + T(0.2) * rnoise(3, T(0.4) * p);
    //T v = std::max(0.2f * p.y - T(0.5) * rnoise(2, T(0.2) * p.xz()), T(0.0)) + T(0.4) * rnoise(2, T(0.2) * p);
    //T v = rnoise(2, T(0.2) * p * vec3_t<T>(1.0, 2.0, 1.0));
    T v = p.y - T(0.4) * rnoise(4, T(0.2) * p.xz());
    return v;
}

#include "quat.h"

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i += 1)
	{
		if (strcmp(argv[i], "--width") == 0 && i + 1 < argc)
		{
			window_width = atoi(argv[i + 1]);
			i += 1; 
		}
		else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc)
		{
			window_height = atoi(argv[i + 1]);
			i += 1; 
		}
		else if (strcmp(argv[i], "--fullscreen") == 0)
		{
			window_fullscreen = true;
		}
		else if (strcmp(argv[i], "--help") == 0)
		{
            printf("usage: %s [--fullscreen] [--width <w>] [--height <h>]\n", argv[0]);
			return 0;
		}
        else
        {
            printf("unrecognized option '%s'\n", argv[i]);
            printf("try `%s --help' for a complete list of options.\n", argv[0]);
            return 0;
        }
	}
    window_aspect = (float)window_width/(float)window_height;

    engine_t engine;

    engine.init();

    // show loading screen
    void rendering_emit_fullscreen_quad_batch(const renderl_texture_t &texture);

    const renderl_texture_t *loading_texture = resource_upload_texture("data/images/loading.png");
    rendering_emit_fullscreen_quad_batch(*loading_texture);
    glfwSwapBuffers();


    const renderl_texture_t *grass_texture = resource_upload_texture("data/images/grass.png");
    const renderl_texture_t *dirt_texture = resource_upload_texture("data/images/dirt.png");
    const renderl_texture_t *sand_texture = resource_upload_texture("data/images/sand.png");

    const renderl_texture_t *crate_diffuse = resource_upload_texture("data/images/crate_diffuse.png");
    const renderl_texture_t *crate_normal_map = resource_upload_texture("data/images/crate_normal.png");

    heightmap_t heightmap = create_heightmap(512, 512, vec3_t<>(-100.0f, -10.0f, -100.0f), vec3_t<>( 100.0f, 15.0f,  100.0f));
    renderm_mesh_t heightmap_mesh = create_heightmap_mesh3(heightmap);
    renderl_texture_t heightmap_texture = create_heightmap_texture(heightmap);
    renderl_texture_t heightmap_normal_map = create_heightmap_normal_map(heightmap);


    simple_material_t simple_material;
    simple_material.program = resource_upload_program(2, "data/shaders/simple_material.vert", "data/shaders/simple_material.frag");
    simple_material.ambient = vec3_t<>(1.0f, 1.0f, 1.0f);
    simple_material.diffuse = vec3_t<>(1.0f, 1.0f, 1.0f);
    simple_material.specular = vec3_t<>(1.0f, 1.0f, 1.0f);
    simple_material.shininess = 50.0f;
    simple_material.ambient_texture = NULL;
    simple_material.diffuse_texture = crate_diffuse;
    simple_material.specular_texture = NULL;
    simple_material.normal_texture = crate_normal_map;

    terrain_material_t terrain_material;
    terrain_material.grass_texture = grass_texture;
    terrain_material.dirt_texture = dirt_texture;
    terrain_material.sand_texture = sand_texture;
    terrain_material.heightmap_texture = &heightmap_texture;
    terrain_material.heightmap_normal_map = &heightmap_normal_map;
    terrain_material.xyz_low = heightmap.xyz_low;
    terrain_material.xyz_high = heightmap.xyz_high;
    terrain_material.program = resource_upload_program(3, "data/shaders/noise4D.glsl", "data/shaders/terrain_material.vert", "data/shaders/terrain_material.frag");
    terrain_material.diffuse = vec3_t<>(1.0f, 1.0f, 1.0f);
    terrain_material.specular = vec3_t<>(1.0f, 1.0f, 1.0f);
    terrain_material.shininess = 50.0f;

    march_material_t march_material;
    march_material.program = resource_upload_program(3, "data/shaders/noise4D.glsl", "data/shaders/march.vert", "data/shaders/march.frag");

    const renderl_texture_t *grass_straws_texture;
    grass_straws_material_t grass_straws_material;
    renderm_mesh_t grass_straws_mesh;
    renderh_model_t grass_straws_model;
    {
        const float width = 0.7f;
        float positions[] =
        {
            width * cosf(0.0f * M_PI / 3.0f + M_PI), 0.0f, width * sinf(0.0f * M_PI / 3.0f + M_PI),
            width * cosf(0.0f * M_PI / 3.0f       ), 0.0f, width * sinf(0.0f * M_PI / 3.0f       ),
            width * cosf(0.0f * M_PI / 3.0f       ), 1.0f, width * sinf(0.0f * M_PI / 3.0f       ),
            width * cosf(0.0f * M_PI / 3.0f + M_PI), 1.0f, width * sinf(0.0f * M_PI / 3.0f + M_PI),

            width * cosf(1.0f * M_PI / 3.0f + M_PI), 0.0f, width * sinf(1.0f * M_PI / 3.0f + M_PI),
            width * cosf(1.0f * M_PI / 3.0f       ), 0.0f, width * sinf(1.0f * M_PI / 3.0f       ),
            width * cosf(1.0f * M_PI / 3.0f       ), 1.0f, width * sinf(1.0f * M_PI / 3.0f       ),
            width * cosf(1.0f * M_PI / 3.0f + M_PI), 1.0f, width * sinf(1.0f * M_PI / 3.0f + M_PI),

            width * cosf(2.0f * M_PI / 3.0f + M_PI), 0.0f, width * sinf(2.0f * M_PI / 3.0f + M_PI),
            width * cosf(2.0f * M_PI / 3.0f       ), 0.0f, width * sinf(2.0f * M_PI / 3.0f       ),
            width * cosf(2.0f * M_PI / 3.0f       ), 1.0f, width * sinf(2.0f * M_PI / 3.0f       ),
            width * cosf(2.0f * M_PI / 3.0f + M_PI), 1.0f, width * sinf(2.0f * M_PI / 3.0f + M_PI),
        };

        float normals[] =
        {
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,

             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,

             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
             0.0f,  1.0f,  0.0f,
        };

        float tex_coords[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };

        unsigned char indices[] =
        {
            0 * 4 + 0, 0 * 4 + 1, 0 * 4 + 3, 0 * 4 + 3, 0 * 4 + 1, 0 * 4 + 2,
            1 * 4 + 0, 1 * 4 + 1, 1 * 4 + 3, 1 * 4 + 3, 1 * 4 + 1, 2 * 4 + 2,
            2 * 4 + 0, 2 * 4 + 1, 2 * 4 + 3, 2 * 4 + 3, 2 * 4 + 1, 1 * 4 + 2,
        };
        int index_count = sizeof(indices)/sizeof(indices[0]);

        grass_straws_mesh.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(positions));
        grass_straws_mesh.normal_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, normals, sizeof(normals));
        grass_straws_mesh.tangent_buffer.handle = 0;
        grass_straws_mesh.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
        grass_straws_mesh.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
        grass_straws_mesh.index_count = index_count;
        grass_straws_mesh.primitive_type = GL_TRIANGLES;

        grass_straws_texture = resource_upload_texture("data/images/grass_straws.png");

        grass_straws_material.program = resource_upload_program(3, "data/shaders/noise4D.glsl", "data/shaders/grass_straws.vert", "data/shaders/grass_straws.frag");
        grass_straws_material.diffuse_texture = grass_straws_texture;

        grass_straws_model = renderh_simple_model(&grass_straws_mesh, &grass_straws_material);
    }

    renderm_mesh_t cube_mesh = create_cube_mesh();
    renderh_model_t cube_model = renderh_simple_model(&cube_mesh, &simple_material);
    renderh_model_t terrain_model = renderh_simple_model(&heightmap_mesh, &terrain_material);


    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(0.0f, 0.0f, 0.0f);

        render_model_component_t *model = new render_model_component_t;
        model->model = &terrain_model;

        physics_component_t *physics = new physics_component_t;
        physics->rigid_body = engine_t::instance->physics_system.create_rigid_heightmap(heightmap);
        physics->in_system = false;

        meta_entity_t me = meta_entity_t("terrain");
        me.add_component(pos);
        me.add_component(model);
        me.add_component(physics);
    }

    renderm_mesh_t water_mesh = create_water_mesh(1000, 1000);
    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(0.0f, 0.0f, 0.0f);
        render_water_surface_component_t *surface = new render_water_surface_component_t;
        surface->mesh = &water_mesh;

        meta_entity_t me = meta_entity_t("water");
        me.add_component(pos);
        me.add_component(surface);
    }

    /*for (std::vector<renderh_model_t>::const_iterator iter = models.begin(); iter != models.end(); iter++)
    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(0.0f, 0.0f, 0.0f);
        render_model_component_t *model = new render_model_component_t;
        model->model = &(*iter);

        meta_entity_t me = meta_entity_t("model");
        me.add_component(pos);
        me.add_component(model);
    }*/

    //renderm_mesh_t plane_mesh = resource_load_obj_mesh("data/models/serpomobil.obj");
    //renderm_material_t *plane_material = resource_load_mtl_material("data/models/serpomobil.mtl");
    //assert(plane_material);
    //plane_material.program = resource_upload_program(2, "data/shaders/simple_material.vert", "data/shaders/simple_material.frag");
    //plane_material.diffuse_texture = resource_upload_texture("data/models/serpomobil.png");
    //renderh_model_t plane_model = renderh_simple_model(&plane_mesh, plane_material);
    renderh_model_t plane_model = resource_load_obj_model("data/models/serpomobil.obj");

    for (int i = 0; i < 16; i++)
    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(5 * (i % 4), 4, 5 * (i / 4));
        pos->xyz.y = sample_heightmap(heightmap, pos->xyz.x, pos->xyz.z) + 20.0f;

        orientation_component_t *orientation = new orientation_component_t;
        orientation->rotation = quat_t<>(0.0f, 0.0f, 0.0f, 1.0f);

        render_model_component_t *model = new render_model_component_t;
        //model->model = &cube_model;
        model->model = &plane_model;

        physics_component_t *physics = new physics_component_t;
        physics->rigid_body = engine_t::instance->physics_system.create_rigid_cube(0.5f, 1);
        physics->in_system = false;

        meta_entity_t me = meta_entity_t("cube");
        me.add_component(pos);
        me.add_component(orientation);
        me.add_component(model);
        me.add_component(physics);
    }

    for (int i = 0; i < 4; i++)
    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(5, 1 + 1.2 * i, 0);

        orientation_component_t *orientation = new orientation_component_t;
        orientation->rotation = quat_t<>(0.0f, 0.0f, 0.0f, 1.0f);

        render_model_component_t *model = new render_model_component_t;
        model->model = &cube_model;

        physics_component_t *physics = new physics_component_t;
        physics->rigid_body = engine_t::instance->physics_system.create_rigid_cube(0.5f, 1);
        physics->in_system = false;

        meta_entity_t me = meta_entity_t("cube");
        me.add_component(pos);
        me.add_component(orientation);
        me.add_component(model);
        me.add_component(physics);
    }


    for (int z = 0; z < 30; z++)
    {
        for (int x = 0; x < 30; x++)
        {
            position_component_t *pos = new position_component_t;
            pos->xyz = vec3_t<>(0.5f * x - 20, 0.0f, 0.5f * z - 15);
            pos->xyz.x += 0.5f * noise(pos->xyz);
            pos->xyz.z += 0.5f * noise(pos->xyz + vec3_t<>(4.0f, 9.0f, 3.0f));
            pos->xyz.y = sample_heightmap(heightmap, pos->xyz.x, pos->xyz.z);
            render_model_component_t *model = new render_model_component_t;
            model->model = &grass_straws_model;

            //meta_entity_t me = meta_entity_t("grass");
            //me.add_component(pos);
            //me.add_component(model);
        }
    }

    // add a sun!
    {
        directional_light_component_t *light = new directional_light_component_t;
        light->direction = vec3_t<>(-1.0f, -0.5f, 0.0f).normalized();
        light->color = vec3_t<>(1.0f, 1.0f, 1.0f);

        meta_entity_t me = meta_entity_t("sun");
        me.add_component(light);
        me.add_component(new sun_component_t);
    }

    // add a source source
    {
        sound_source_component_t *sound = new sound_source_component_t;
        sound->wave = resource_upload_wave("data/sounds/five-armies.ogg");
        sound->voice = audiol_create_voice();
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(10.0f, 3.0f, 10.0f);

        meta_entity_t me = meta_entity_t("epic music");
        me.add_component(sound);
        me.add_component(pos);
    }

    renderh_camera_t camera;
    camera.position = vec3_t<>(0.0f, 0.0f, 2.0f);
    camera.forward = vec3_t<>(0.0f, 0.0f, -1.0f);
    camera.right = vec3_t<>(1.0f, 0.0f, 0.0f);
    camera.up = vec3_t<>(0.0f, 1.0f, 0.0f);

    renderl_texture_t noise_texture = resource_upload_noise_texture(window_width, window_height);

    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(0.0f, 0.0f, 0.0f);

        orientation_component_t *ori = new orientation_component_t;
        ori->rotation = quat_t<>(0.0f, 0.0f, 0.0f, 1.0f);

        lens_component_t *lens = new lens_component_t;
        lens->fov = 45.0f * M_PI / 180.0f;
        lens->aspect = float(window_width) / float(window_height);
        lens->near = 0.1f;
        lens->far = 100.0f;

        player_component_t *player = new player_component_t;

        point_light_component_t *light = new point_light_component_t;
        light->color = vec3_t<>(1.0f, 1.0f, 1.0f);

        physics_component_t *physics = new physics_component_t;
        physics->rigid_body = engine_t::instance->physics_system.create_rigid_sphere(0.5f, 0);
        physics->in_system = false;

        meta_entity_t me("player");
        me.add_component(pos);
        me.add_component(ori);
        me.add_component(lens);
        me.add_component(player);
        me.add_component(light);
        me.add_component(physics);
    }

    {
        position_component_t *pos = new position_component_t;
        pos->xyz = vec3_t<>(0.0f, 0.0f, 0.0f);
        pos->xyz.y = sample_heightmap(heightmap, pos->xyz.x, pos->xyz.z) + 2.5f;

        orientation_component_t *ori = new orientation_component_t;
        ori->rotation = quat_t<>(0.0f, 0.0f, 0.0f, 1.0f);
        ori->rotation = quat_t<>(vec3_t<>(0.0f, 0.0f, 1.0f), -M_PI / 4.0f) * ori->rotation;

        spot_light_component_t *light = new spot_light_component_t;
        light->color = vec3_t<>(1.0f, 1.0f, 1.0f);

        lens_component_t *lens = new lens_component_t;
        lens->fov = 45.0f * M_PI / 180.0f;
        lens->aspect = float(window_width) / float(window_height);
        lens->near = 0.1f;
        lens->far = 100.0f;

        shadow_caster_component_t *shadow = new shadow_caster_component_t;
        renderl_frame_buffer_t *fbo = new renderl_frame_buffer_t;
        *fbo = renderl_create_frame_buffer(512, 512, 1, GL_RGBA, false);
        shadow->shadow_fbo = fbo;

        meta_entity_t me("spotlight");
        me.add_component(pos);
        me.add_component(ori);
        me.add_component(light);
        me.add_component(lens);
        me.add_component(shadow);
    }

    engine.run();

    return 0;
}


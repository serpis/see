#include <cstdlib>
#include <cmath>

#include "material.hpp"
#include "renderm.hpp"
#include "math.hpp"

extern double time_now;
extern bool use_normal_mapping;

void simple_material_t::fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } vertex_parameters;

    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        vec3_t<> ambient;
        float dummy0;
        vec3_t<> diffuse;
        float dummy1;
        vec3_t<> specular;
        float shininess;
        int has_ambient_map;
        int has_diffuse_map;
        int has_specular_map;
        int has_normal_map;
        vec2_t<> normal_map_dimensions;
    } fragment_parameters;

    batch->program = this->program;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;

    fragment_parameters.projection = eye.projection;
    fragment_parameters.view = eye.view;
    fragment_parameters.model = model_matrix;
    fragment_parameters.ambient = this->ambient;
    fragment_parameters.dummy0 = 0.0f;
    fragment_parameters.diffuse = this->diffuse;
    fragment_parameters.dummy1 = 0.0f;
    fragment_parameters.specular = this->specular;
    //fragment_parameters.dummy2 = 0.0f;
    fragment_parameters.shininess = this->shininess;
    fragment_parameters.has_ambient_map = this->ambient_texture != NULL;
    fragment_parameters.has_diffuse_map = this->diffuse_texture != NULL;
    fragment_parameters.has_specular_map = this->specular_texture != NULL;
    fragment_parameters.has_normal_map = this->normal_texture != NULL;
    if (this->normal_texture)
    {
        fragment_parameters.normal_map_dimensions.x = this->normal_texture->width;
        fragment_parameters.normal_map_dimensions.y = this->normal_texture->height;
    }

    batch->vertex_parameters = &vertex_parameters;
    batch->vertex_parameters_size = sizeof(vertex_parameters);
    batch->fragment_parameters = &fragment_parameters;
    batch->fragment_parameters_size = sizeof(fragment_parameters);

    int next_index = 0;
    if (this->ambient_texture)
    {
        batch->textures[next_index++] = this->ambient_texture;
    }
    if (this->diffuse_texture)
    {
        batch->textures[next_index++] = this->diffuse_texture;
    }
    if (this->specular_texture)
    {
        batch->textures[next_index++] = this->specular_texture;
    }
    if (this->normal_texture)
    {
        batch->textures[next_index++] = this->normal_texture;
    }
    batch->texture_count = next_index;

    batch->vertex_buffer_count = 4;
    batch->vertex_buffers[0] = &mesh.position_buffer;
    batch->vertex_buffers[1] = &mesh.normal_buffer;
    batch->vertex_buffers[2] = &mesh.tangent_buffer;
    batch->vertex_buffers[3] = &mesh.tex_coord_buffer;
    batch->index_buffer = &mesh.index_buffer;
    batch->index_count = mesh.index_count;
    batch->primitive_type = mesh.primitive_type;

    batch->use_depth_test = true;
    batch->use_back_face_culling = true;
    batch->use_blending = false;
}

void grass_straws_material_t::fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        float time;
    } vertex_parameters;

    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } fragment_parameters;

    batch->program = this->program;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;
    vertex_parameters.time = time_now;

    fragment_parameters.projection = eye.projection;
    fragment_parameters.view = eye.view;
    fragment_parameters.model = model_matrix;

    batch->vertex_parameters = &vertex_parameters;
    batch->vertex_parameters_size = sizeof(vertex_parameters);
    batch->fragment_parameters = &fragment_parameters;
    batch->fragment_parameters_size = sizeof(fragment_parameters);

    batch->texture_count = 1;
    batch->textures[0] = this->diffuse_texture;

    batch->vertex_buffer_count = 3;
    batch->vertex_buffers[0] = &mesh.position_buffer;
    batch->vertex_buffers[1] = &mesh.normal_buffer;
    batch->vertex_buffers[2] = &mesh.tex_coord_buffer;
    batch->index_buffer = &mesh.index_buffer;
    batch->index_count = mesh.index_count;
    batch->primitive_type = mesh.primitive_type;

    batch->use_depth_test = true;
    batch->use_back_face_culling = false;
    batch->use_blending = false;
}

void terrain_material_t::fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        vec3_t<> xyz_low;
        float dummy0;
        vec3_t<> xyz_high;
    } vertex_parameters;

    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        vec3_t<> diffuse;
        float dummy0;
        vec3_t<> specular;
        float shininess;
    } fragment_parameters;

    batch->program = this->program;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;
    vertex_parameters.xyz_low = this->xyz_low;
    vertex_parameters.xyz_high = this->xyz_high;

    fragment_parameters.projection = eye.projection;
    fragment_parameters.view = eye.view;
    fragment_parameters.model = model_matrix;
    fragment_parameters.diffuse = this->diffuse;
    fragment_parameters.specular = this->specular;
    fragment_parameters.shininess = this->shininess;

    batch->vertex_parameters = &vertex_parameters;
    batch->vertex_parameters_size = sizeof(vertex_parameters);
    batch->fragment_parameters = &fragment_parameters;
    batch->fragment_parameters_size = sizeof(fragment_parameters);

    batch->texture_count = 5;
    batch->textures[0] = this->grass_texture;
    batch->textures[1] = this->dirt_texture;
    batch->textures[2] = this->sand_texture;
    batch->textures[3] = this->heightmap_texture;
    batch->textures[4] = this->heightmap_normal_map;

    batch->vertex_buffer_count = 2;
    batch->vertex_buffers[0] = &mesh.position_buffer;
    batch->vertex_buffers[1] = &mesh.normal_buffer;
    batch->index_buffer = &mesh.index_buffer;
    batch->index_count = mesh.index_count;
    batch->primitive_type = mesh.primitive_type;

    batch->use_depth_test = true;
    batch->use_back_face_culling = true;
    batch->use_blending = false;
}

void march_material_t::fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } vertex_parameters;

    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        float time;
    } fragment_parameters;

    batch->program = this->program;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;

    fragment_parameters.projection = eye.projection;
    fragment_parameters.view = eye.view;
    fragment_parameters.model = model_matrix;
    fragment_parameters.time = time_now;

    batch->vertex_parameters = &vertex_parameters;
    batch->vertex_parameters_size = sizeof(vertex_parameters);
    batch->fragment_parameters = &fragment_parameters;
    batch->fragment_parameters_size = sizeof(fragment_parameters);

    batch->texture_count = 0;

    batch->vertex_buffer_count = 3;
    batch->vertex_buffers[0] = &mesh.position_buffer;
    batch->vertex_buffers[1] = &mesh.normal_buffer;
    batch->vertex_buffers[2] = &mesh.tex_coord_buffer;
    batch->index_buffer = &mesh.index_buffer;
    batch->index_count = mesh.index_count;
    batch->primitive_type = mesh.primitive_type;

    batch->use_depth_test = true;
    batch->use_back_face_culling = true;
    batch->use_blending = false;
}

void water_material_t::fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } vertex_parameters;

    static struct
    {
        float t;
    } fragment_parameters;

    batch->program = this->program;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;

    fragment_parameters.t = (float)time_now;

    batch->vertex_parameters = &vertex_parameters;
    batch->vertex_parameters_size = sizeof(vertex_parameters);
    batch->fragment_parameters = &fragment_parameters;
    batch->fragment_parameters_size = sizeof(fragment_parameters);

    batch->texture_count = 0;

    batch->vertex_buffer_count = 2;
    batch->vertex_buffers[0] = &mesh.position_buffer;
    batch->vertex_buffers[1] = &mesh.normal_buffer;
    batch->index_buffer = &mesh.index_buffer;
    batch->index_count = mesh.index_count;
    batch->primitive_type = mesh.primitive_type;

    batch->use_depth_test = true;
    batch->use_back_face_culling = true;
    batch->use_blending = false;
}


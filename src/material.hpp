#ifndef _MATERIAL_HPP
#define _MATERIAL_HPP

#include "renderl.hpp"
#include "renderm.hpp"

struct simple_material_t : public renderm_material_t
{
    vec3_t<> ambient;
    vec3_t<> diffuse;
    vec3_t<> specular;
    float shininess;
    const renderl_texture_t *ambient_texture;
    const renderl_texture_t *diffuse_texture;
    const renderl_texture_t *specular_texture;
    const renderl_texture_t *normal_texture;
    void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const;
};

struct grass_straws_material_t : public renderm_material_t
{
    const renderl_texture_t *diffuse_texture;
    void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const;
};

struct terrain_material_t : public renderm_material_t
{
    vec3_t<> diffuse;
    vec3_t<> specular;
    float shininess;
    vec3_t<> xyz_low;
    vec3_t<> xyz_high;
    const renderl_texture_t *grass_texture;
    const renderl_texture_t *dirt_texture;
    const renderl_texture_t *sand_texture;
    const renderl_texture_t *heightmap_texture;
    const renderl_texture_t *heightmap_normal_map;
    void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const;
};

struct march_material_t : public renderm_material_t
{
    void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const;
};

struct water_material_t : public renderm_material_t
{
    void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const;
};

#endif


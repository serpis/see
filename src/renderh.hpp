#ifndef _RENDERH_HPP
#define _RENDERH_HPP

#include "renderl.hpp"
#include "renderm.hpp"

struct renderh_model_t
{
    const renderm_mesh_t *meshes[8];
    const renderm_material_t *materials[8];
    int mesh_count;
};

struct renderh_camera_t
{
    vec3_t<> position;
    vec3_t<> forward;
    vec3_t<> right;
    vec3_t<> up;
};

void renderh_init();
renderh_model_t renderh_simple_model(const renderm_mesh_t *mesh, const renderm_material_t *material);
renderh_model_t renderh_load_obj(const char *filename);
void renderh_emit_model_batches(const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderh_model_t &model);
void renderh_emit_fullscreen_quad_batch(const renderl_texture_t &texture);
void renderh_emit_ssao_fullscreen_quad_batch(const renderl_texture_t &depth_texture);
renderm_eye_t renderh_camera_to_eye(const renderh_camera_t &camera);

#endif // _RENDERH_HPP


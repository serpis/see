#ifndef _RENDERM_HPP
#define _RENDERM_HPP

#include "renderl.hpp"
#include "math.hpp"

struct renderm_mesh_t
{
    renderl_vertex_buffer_t position_buffer;
    renderl_vertex_buffer_t normal_buffer;
    renderl_vertex_buffer_t tangent_buffer;
    renderl_vertex_buffer_t tex_coord_buffer;
    renderl_index_buffer_t index_buffer;
    int index_count;
    int primitive_type;
};

struct renderm_eye_t
{
    mat4_t<> projection;
    mat4_t<> view;
};

struct renderm_material_t
{
    const renderl_program_t *program;
    virtual void fill_batch(renderl_batch_t *batch, const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderm_mesh_t &mesh) const = 0;
};

#endif // _RENDERM_HPP


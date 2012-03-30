#include <cmath>
#include <cassert>

#ifdef _OSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "renderh.hpp"
#include "resources.hpp"

extern float window_aspect;

void renderh_init()
{
}

renderh_model_t renderh_simple_model(const renderm_mesh_t *mesh, const renderm_material_t *material)
{
    renderh_model_t model;
    model.meshes[0] = mesh;
    model.materials[0] = material;
    model.mesh_count = 1;
    return model;
}

void renderh_emit_model_batches(const renderm_eye_t &eye, const mat4_t<> &model_matrix, const renderh_model_t &model)
{
    for (int i = 0; i < model.mesh_count; i++)
    {
        renderl_batch_t batch = create_default_batch();

        const renderm_mesh_t &mesh = *model.meshes[i];
        const renderm_material_t &material = *model.materials[i];

        // ask material to fill in fields about program, uniforms, textures and mesh fields
        material.fill_batch(&batch, eye, model_matrix, mesh);

        renderl_push_batch(batch);
    }
}

renderm_eye_t renderh_camera_to_eye(const renderh_camera_t &camera)
{
    renderm_eye_t eye;
    eye.projection = mat4_t<>::perspective(60.0f * M_PI / 180.0f, window_aspect, 0.1f, 300.0f);
    eye.view = mat4_t<>::lookat(camera.position, camera.position + camera.forward, camera.up);

    return eye;
}


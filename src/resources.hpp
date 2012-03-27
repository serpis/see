#ifndef _RESOURCES_HPP
#define _RESOURCES_HPP

#include <vector>

#include "renderm.hpp"
#include "material.hpp"

void resource_init();
renderm_mesh_t resource_load_obj_mesh(const char *filename);
renderm_material_t *resource_load_mtl_material(const char *filename);
renderh_model_t resource_load_obj_model(const char *filename);
const renderl_texture_t *resource_upload_texture(const char *filename);
renderl_texture_t resource_upload_noise_texture(int width, int height);
const renderl_program_t *resource_upload_program(int shader_count, ...);
const class audiol_wave_t *resource_upload_wave(const char *filename);

void resource_load_obj_models(const char *filename, std::vector<renderh_model_t> *models);

#endif // _RESOURCES_HPP


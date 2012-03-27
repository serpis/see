#ifndef _RENDERL_HPP
#define _RENDERL_HPP

struct renderl_source_t
{
    int type;
    const char *name;
    const char *source;
    int source_size;
};

struct renderl_program_t
{
    int handle;
};

struct renderl_texture_t
{
    unsigned int handle;
    int width, height;
};

struct renderl_frame_buffer_t
{
    unsigned int handle;
    int width, height;
    int texture_count;
    renderl_texture_t textures[8];
    renderl_texture_t depth_texture;
};

struct renderl_vertex_buffer_t
{
    unsigned int handle;
    int type;
    int component_count;
};

struct renderl_index_buffer_t
{
    unsigned int handle;
    int type;
};

struct renderl_uniform_buffer_t
{
    unsigned int handle;
};

struct renderl_batch_t
{
    const struct renderl_program_t *program;

    const void *vertex_parameters;
    int vertex_parameters_size;
    const void *fragment_parameters;
    int fragment_parameters_size;

    int texture_count;
    const renderl_texture_t *textures[8];

    int vertex_buffer_count;
    const struct renderl_vertex_buffer_t *vertex_buffers[8];
    const struct renderl_index_buffer_t *index_buffer;
    int index_count;

    int primitive_type;

    bool use_depth_test;
    bool use_back_face_culling;
    bool use_blending;
    int src_blend_func;
    int dst_blend_func;
};

void render_print_errors(const char *scope);
void renderl_init();

renderl_batch_t create_default_batch();

renderl_texture_t renderl_upload_texture(int width, int height, int source_format, void *data);
renderl_texture_t renderl_upload_texture_adv(int width, int height, int target_format, int source_format, int source_type, void *data);
renderl_frame_buffer_t renderl_create_frame_buffer(int width, int height, int color_attachment_count, int color_format, bool stencil_buffer);
void renderl_delete_texture(renderl_texture_t texture);
renderl_frame_buffer_t renderl_assemble_frame_buffer(int width, int height, const renderl_texture_t &depth_texture, int color_attachment_count, const renderl_texture_t *color_textures);
renderl_program_t renderl_upload_program(int shader_source_count, const renderl_source_t *shader_sources);
void renderl_delete_program(renderl_program_t program);
renderl_vertex_buffer_t renderl_upload_vertex_buffer(int type, int component_count, const void *data, int size);
renderl_index_buffer_t renderl_upload_index_buffer(int type, const void *data, int size);
renderl_uniform_buffer_t renderl_upload_uniform_buffer(const void *data, int size);
void renderl_update_uniform_buffer(renderl_uniform_buffer_t &uniform_buffer, const void *data, int size);
void renderl_push_batch(const renderl_batch_t &batch);
void renderl_bind_frame_buffer(const renderl_frame_buffer_t *fbo);

#endif // _RENDERL_HPP

/*
struct renderapi_command_batch 
{ 
    struct renderapi_command_header header; 
 
    struct renderapi_sampler_state samplers[RENDERAPI_MAX_SAMPLERS]; 
 
    struct renderapi_program* program; 
    const void* vp_params; 
    const void* fp_params; 
    uint32_t vp_num_param_bytes; 
    uint32_t fp_num_param_bytes; 
 
    const struct renderapi_vertex_layout_elem* vertex_layout; 
    uint16_t vertex_layout_stride; 
    uint8_t num_vertex_layout_elems; 
 
    struct renderapi_buffer* vertex_buffer; 
    struct renderapi_buffer* index_buffer; 
    enum renderapi_data_type index_type; 
    uint16_t index_offset; 
    uint16_t num_indices; 
     
    enum renderapi_primitive_type prim_type : 1; 
     
    enum renderapi_winding_order wind : 1; 
    enum renderapi_cull_mode cull : 2; 
    enum renderapi_depth_func depth_func : 4; 
};
*/


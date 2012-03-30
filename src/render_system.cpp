#include <cstdio>
#include <cmath>

#include <GL/glew.h>
#ifdef _OSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "material.hpp"
#include "renderl.hpp"
#include "renderm.hpp"
#include "renderh.hpp"
#include "rendering.hpp"
#include "resources.hpp"
#include "components.hpp"
#include "render_system.hpp"
#include "entity_system.hpp"

#include "engine.hpp"

extern double time_now;
extern int mouse_x;
extern int mouse_y;

static renderl_frame_buffer_t pre_deferred_fbo;
static renderl_frame_buffer_t post_deferred_fbo;
static renderl_frame_buffer_t render_water_fbo;

extern int window_width;
extern int window_height;
extern float window_aspect;

static const renderl_program_t *picking_program;
static const renderl_program_t *water_program;
static const renderl_program_t *apply_light_program;
static const renderl_program_t *ssao_program;

static renderl_texture_t dummy_texture0;
static renderl_texture_t dummy_texture1;

static renderm_mesh_t skydome_mesh;
static const renderl_program_t *skydome_program;
static const renderl_texture_t *skydome_texture;


static const renderl_texture_t *speaker_texture;

static struct
{
    const renderl_program_t *program;
    renderl_vertex_buffer_t position_buffer;
    renderl_vertex_buffer_t color_buffer;
    renderl_index_buffer_t index_buffer;
    int index_count;
    int primitive_type;
} debug_lines;

static struct
{
    const renderl_program_t *program;
    renderl_vertex_buffer_t position_buffer;
    renderl_vertex_buffer_t tex_coord_buffer;
    renderl_index_buffer_t index_buffer;
    int index_count;
    int primitive_type;
} billboard;

static struct
{
    const renderl_program_t *program;
    renderl_vertex_buffer_t position_buffer;
    renderl_vertex_buffer_t tex_coord_buffer;
    renderl_index_buffer_t index_buffer;
    int index_count;
    int primitive_type;
} fullscreen_quad;

extern renderm_mesh_t create_cube_mesh();



void render_system_t::init()
{
    pre_deferred_fbo = renderl_create_frame_buffer(window_width, window_height, 4, GL_RGBA16F, true);
    {
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);

        dummy_texture0.handle = tex;
        dummy_texture0.width = window_width;
        dummy_texture0.height = window_height;

        //render_water_fbo = renderl_assemble_frame_buffer(window_width, window_height, pre_deferred_fbo.depth_texture, 1, &post_deferred_fbo.textures[0]);
        post_deferred_fbo = renderl_assemble_frame_buffer(window_width, window_height, pre_deferred_fbo.depth_texture, 1, &dummy_texture0);
    }
    {
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);

        dummy_texture1.handle = tex;
        dummy_texture1.width = window_width;
        dummy_texture1.height = window_height;

        //render_water_fbo = renderl_assemble_frame_buffer(window_width, window_height, pre_deferred_fbo.depth_texture, 1, &post_deferred_fbo.textures[0]);
        render_water_fbo = renderl_assemble_frame_buffer(window_width, window_height, pre_deferred_fbo.depth_texture, 1, &dummy_texture1);
    }

    picking_program = resource_upload_program(2, "data/shaders/picking.vert", "data/shaders/picking.frag");
    water_program = resource_upload_program(3, "data/shaders/noise4D.glsl", "data/shaders/water.vert", "data/shaders/water.frag");
    apply_light_program = resource_upload_program(2, "data/shaders/apply_light.vert", "data/shaders/apply_light.frag");

    {
        float positions[] =
        {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f,
        };
        float tex_coords[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
        };
        unsigned char indices[] = { 0, 1, 2, 3 };

        fullscreen_quad.program = resource_upload_program(2, "data/shaders/fullscreen_quad.vert", "data/shaders/fullscreen_quad.frag");
        fullscreen_quad.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, positions, sizeof(positions));
        fullscreen_quad.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, tex_coords, sizeof(tex_coords));
        fullscreen_quad.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
        fullscreen_quad.index_count = sizeof(indices)/sizeof(indices[0]);
        fullscreen_quad.primitive_type = GL_TRIANGLE_STRIP;
    }
    ssao_program = resource_upload_program(2, "data/shaders/ssao.vert", "data/shaders/ssao.frag");

    {
        float positions[] =
        {
            0.0f, 0.0f, 0.0f,
            0.2f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.2f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.2f,
        };
        float colors[] =
        {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
        };
        unsigned char indices[] = { 0, 1, 2, 3, 4, 5 };

        debug_lines.program = resource_upload_program(2, "data/shaders/debug_lines.vert", "data/shaders/debug_lines.frag");
        debug_lines.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, positions, sizeof(positions));
        debug_lines.color_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 3, colors, sizeof(colors));
        debug_lines.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
        debug_lines.index_count = sizeof(indices)/sizeof(indices[0]);
        debug_lines.primitive_type = GL_LINES;
    }

    {
        float positions[] =
        {
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f,
        };
        float texture_coords[] =
        {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };
        unsigned char indices[] = { 0, 1, 2, 3 };

        billboard.program = resource_upload_program(2, "data/shaders/billboard.vert", "data/shaders/billboard.frag");
        billboard.position_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, positions, sizeof(positions));
        billboard.tex_coord_buffer = renderl_upload_vertex_buffer(GL_FLOAT, 2, texture_coords, sizeof(texture_coords));
        billboard.index_buffer = renderl_upload_index_buffer(GL_UNSIGNED_BYTE, indices, sizeof(indices));
        billboard.index_count = sizeof(indices)/sizeof(indices[0]);
        billboard.primitive_type = GL_TRIANGLE_FAN;
    }

    skydome_mesh = create_cube_mesh();
    skydome_program = resource_upload_program(2, "data/shaders/skydome.vert", "data/shaders/skydome.frag");
    skydome_texture = resource_upload_texture("data/images/skydome.png");

    speaker_texture = resource_upload_texture("data/images/speaker-icon.png");
}

struct item_t
{
    int entity;
    mat4_t<> model_matrix;
    const renderh_model_t *model;
};

struct light_t
{
    // pointlight = 0
    // spotlight = 1
    // directional = 2
    int type;
    // valid when type == 1
    mat4_t<> light_view;
    // valid when type == 1
    mat4_t<> view_to_light_view;
    // valid when type == 1
    mat4_t<> light_projection;
    // valid when type == 0 or type == 1
    vec3_t<> position;
    // valid when type == 2
    vec3_t<> direction;
    vec3_t<> color;
    const renderl_texture_t *shadow_map;
    const renderl_frame_buffer_t *shadow_fbo;
};


renderm_eye_t *HAX_camera_eye;
static std::list<item_t> *HAX_items;
static std::list<light_t> *HAX_lights;
static vec3_t<> HAX_light_direction;

static void extract_visible_stuff(const renderh_camera_t &camera, std::list<item_t> *items, std::list<light_t> *lights)
{
    // later it might be wise to use a smarter extract function. maybe even with frustum culling?!

    // :(
    HAX_items = items;
    HAX_lights = lights;

    entity_manager_t::default_manager->iterate_nodes<render_model_component_t, position_component_t, orientation_component_t>(2, [](int entity, render_model_component_t *model_component, position_component_t *pos, orientation_component_t *orientation)
    {
        item_t item;
        item.entity = entity;
        item.model_matrix = mat4_t<>::translation(pos->xyz);
        if (orientation)
        {
            item.model_matrix *= orientation->rotation.rotation_matrix();
        }
        item.model = model_component->model;

        HAX_items->push_back(item);
    });
    entity_manager_t::default_manager->iterate_nodes<point_light_component_t, position_component_t>(2, [](point_light_component_t *light_component, position_component_t *position)
    {
        light_t light;
        light.type = 0;
        light.position = position->xyz;
        light.color = light_component->color;
        light.shadow_map = NULL;
        light.shadow_fbo = NULL;

        HAX_lights->push_back(light);
    });
    entity_manager_t::default_manager->iterate_nodes<spot_light_component_t, position_component_t, orientation_component_t, lens_component_t, shadow_caster_component_t>(4, [](spot_light_component_t *light_component, position_component_t *position, orientation_component_t *orientation, lens_component_t *lens, shadow_caster_component_t *shadow)
    {
        renderh_camera_t camera;
        camera.position = position->xyz;
        camera.forward = orientation->rotation.forward();
        camera.right = orientation->rotation.right();
        camera.up = orientation->rotation.up();

        renderm_eye_t light_eye = renderh_camera_to_eye(camera);

        if (engine_t::instance->input_system.keys['L'])
        {
            *HAX_camera_eye = light_eye;
        }

        light_t light;
        light.type = 1;
        light.light_view = light_eye.view;
        light.view_to_light_view = light_eye.view * HAX_camera_eye->view.inverted();
        light.light_projection = light_eye.projection;
        light.position = position->xyz;
        light.color = light_component->color;
        if (shadow == NULL)
        {
            light.shadow_fbo = NULL;
        }
        else
        {
            light.shadow_fbo = shadow->shadow_fbo;
        }

        HAX_lights->push_back(light);
    });
    entity_manager_t::default_manager->iterate_nodes<directional_light_component_t>(1, [](directional_light_component_t *light_component)
    {
        light_t light;
        light.type = 2;
        light.direction = light_component->direction;
        light.color = light_component->color;
        light.shadow_map = NULL;
        light.shadow_fbo = NULL;

        HAX_lights->push_back(light);
    });
}


void rendering_emit_fullscreen_quad_batch(const renderl_texture_t &texture)
{
    renderl_batch_t batch = create_default_batch();

    batch.program = fullscreen_quad.program;

    batch.vertex_parameters = NULL;
    batch.vertex_parameters_size = 0;
    batch.fragment_parameters = NULL;
    batch.fragment_parameters_size = 0;

    batch.texture_count = 1;
    batch.textures[0] = &texture;

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &fullscreen_quad.position_buffer;
    batch.vertex_buffers[1] = &fullscreen_quad.tex_coord_buffer;
    batch.index_buffer = &fullscreen_quad.index_buffer;
    batch.index_count = fullscreen_quad.index_count;
    batch.primitive_type = fullscreen_quad.primitive_type;

    batch.use_depth_test = false;

    renderl_push_batch(batch);
}

void rendering_emit_ssao_fullscreen_quad_batch(const renderl_texture_t &depth_texture)
{
    renderl_batch_t batch = create_default_batch();

    batch.program = ssao_program;

    batch.vertex_parameters = NULL;
    batch.vertex_parameters_size = 0;
    batch.fragment_parameters = NULL;
    batch.fragment_parameters_size = 0;

    batch.texture_count = 1;
    batch.textures[0] = &depth_texture;

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &fullscreen_quad.position_buffer;
    batch.vertex_buffers[1] = &fullscreen_quad.tex_coord_buffer;
    batch.index_buffer = &fullscreen_quad.index_buffer;
    batch.index_count = fullscreen_quad.index_count;
    batch.primitive_type = fullscreen_quad.primitive_type;

    batch.use_depth_test = false;

    renderl_push_batch(batch);
}

static void rendering_emit_apply_light_batch(const renderm_eye_t &eye, const light_t &light, const renderl_texture_t &position_texture, const renderl_texture_t &normal_texture, const renderl_texture_t &diffuse_texture, const renderl_texture_t &specular_texture, const renderl_texture_t *depth_texture)
{
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
    } vertex_parameters;

    static struct
    {
        mat4_t<> view;
        mat4_t<> view_to_light_view;
        mat4_t<> light_projection;
        int type;
        float dummya;
        float dummyb;
        float dummca;
        vec3_t<> light_position;
        float dummy0;
        vec3_t<> light_direction;
        float dummy1;
        vec3_t<> light_color;
        float dummy2;
    } fragment_parameters;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;

    fragment_parameters.view = eye.view;
    fragment_parameters.type = light.type;
    fragment_parameters.view_to_light_view = light.view_to_light_view;
    fragment_parameters.light_projection = light.light_projection;
    fragment_parameters.light_position = light.position;
    fragment_parameters.light_direction = light.direction;
    fragment_parameters.light_color = light.color;

    renderl_batch_t batch = create_default_batch();

    batch.program = apply_light_program;

    batch.vertex_parameters = &vertex_parameters;
    batch.vertex_parameters_size = sizeof(vertex_parameters);
    batch.fragment_parameters = &fragment_parameters;
    batch.fragment_parameters_size = sizeof(fragment_parameters);

    batch.texture_count = 4;
    batch.textures[0] = &position_texture;
    batch.textures[1] = &normal_texture;
    batch.textures[2] = &diffuse_texture;
    batch.textures[3] = &specular_texture;
    if (depth_texture != NULL)
    {
        batch.texture_count = 5;
        batch.textures[4] = depth_texture;
    }

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &fullscreen_quad.position_buffer;
    batch.vertex_buffers[1] = &fullscreen_quad.tex_coord_buffer;
    batch.index_buffer = &fullscreen_quad.index_buffer;
    batch.index_count = fullscreen_quad.index_count;
    batch.primitive_type = fullscreen_quad.primitive_type;

    batch.use_depth_test = false;
    batch.use_blending = true;
    batch.src_blend_func = GL_ONE;
    batch.dst_blend_func = GL_ONE;

    renderl_push_batch(batch);
}


static void renderer_emit_picking_id_batches(const renderm_eye_t &eye, const mat4_t<> &model_matrix, int id, const renderh_model_t &model)
{
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } vertex_parameters;

    static struct
    {
        float idr, idg, idb, ida;
    } fragment_parameters;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;

    int idr = id % 256;
    id /= 256;
    int idg = id % 256;
    id /= 256;
    int idb = id % 256;
    id /= 256;
    int ida = id % 256;
    id /= 256;

    fragment_parameters.idr = idr / 255.0f;
    fragment_parameters.idg = idg / 255.0f;
    fragment_parameters.idb = idb / 255.0f;
    fragment_parameters.ida = ida / 255.0f;

    for (int i = 0; i < model.mesh_count; i++)
    {
        const renderm_mesh_t &mesh = *model.meshes[i];

        renderl_batch_t batch = create_default_batch();

        batch.program = picking_program;

        batch.vertex_parameters = &vertex_parameters;
        batch.vertex_parameters_size = sizeof(vertex_parameters);
        batch.fragment_parameters = &fragment_parameters;
        batch.fragment_parameters_size = sizeof(fragment_parameters);

        batch.texture_count = 0;

        batch.vertex_buffer_count = 1;
        batch.vertex_buffers[0] = &mesh.position_buffer;
        batch.index_buffer = &mesh.index_buffer;
        batch.index_count = mesh.index_count;
        batch.primitive_type = mesh.primitive_type;

        batch.use_depth_test = true;
        batch.use_blending = false;
        batch.use_back_face_culling = true;

        renderl_push_batch(batch);
    }
}

static void renderer_emit_billboard_batch(const renderm_eye_t &eye, const vec3_t<> &position, const renderl_texture_t *texture)
{
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        vec3_t<> offset;
        float dummy0;
    } vertex_parameters;

    //vec4_t<> proj_pos = eye.projection * eye.view * vec4_t<>(position, 1.0);
    //proj_pos = proj_pos * (1.0f / proj_pos.w);

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.offset = position;

    renderl_batch_t batch = create_default_batch();

    batch.program = billboard.program;

    batch.vertex_parameters = &vertex_parameters;
    batch.vertex_parameters_size = sizeof(vertex_parameters);
    batch.fragment_parameters = NULL;
    batch.fragment_parameters_size = 0;

    batch.texture_count = 1;
    batch.textures[0] = texture;

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &billboard.position_buffer;
    batch.vertex_buffers[1] = &billboard.tex_coord_buffer;
    batch.index_buffer = &billboard.index_buffer;
    batch.index_count = billboard.index_count;
    batch.primitive_type = billboard.primitive_type;

    batch.use_depth_test = true;
    batch.use_blending = false;
    batch.use_back_face_culling = false;

    renderl_push_batch(batch);
}

static void renderer_emit_debug_crosshair_batch(const renderm_eye_t &eye, const mat4_t<> &model_matrix)
{
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
    } vertex_parameters;

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model_matrix;

    renderl_batch_t batch = create_default_batch();

    batch.program = debug_lines.program;

    batch.vertex_parameters = &vertex_parameters;
    batch.vertex_parameters_size = sizeof(vertex_parameters);
    batch.fragment_parameters = NULL;
    batch.fragment_parameters_size = 0;

    batch.texture_count = 0;

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &debug_lines.position_buffer;
    batch.vertex_buffers[1] = &debug_lines.color_buffer;
    batch.index_buffer = &debug_lines.index_buffer;
    batch.index_count = debug_lines.index_count;
    batch.primitive_type = debug_lines.primitive_type;

    batch.use_depth_test = false;
    batch.use_blending = false;
    batch.use_back_face_culling = false;

    renderl_push_batch(batch);
}

static void renderer_emit_draw_skydome_batch(const renderm_eye_t &eye)
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
    } vertex_parameters;

    static struct
    {
        float dummy;
    } fragment_parameters;

    renderl_batch_t batch = create_default_batch();

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;

    batch.program = skydome_program;

    batch.vertex_parameters = &vertex_parameters;
    batch.vertex_parameters_size = sizeof(vertex_parameters);
    batch.fragment_parameters = &fragment_parameters;
    batch.fragment_parameters_size = sizeof(fragment_parameters);

    batch.texture_count = 1;
    batch.textures[0] = skydome_texture;

    batch.vertex_buffer_count = 1;
    batch.vertex_buffers[0] = &skydome_mesh.position_buffer;
    batch.index_buffer = &skydome_mesh.index_buffer;
    batch.index_count = skydome_mesh.index_count;
    batch.primitive_type = skydome_mesh.primitive_type;

    batch.use_depth_test = false;
    batch.use_blending = false;
    batch.use_back_face_culling = false;

    renderl_push_batch(batch);
}

static void renderer_emit_draw_water_batch(const renderm_eye_t &eye, const vec3_t<> &light_direction, const mat4_t<> &model, const renderm_mesh_t &water_mesh)
{
    // investigate why this needs to be static??
    static struct
    {
        mat4_t<> projection;
        mat4_t<> view;
        mat4_t<> model;
        vec3_t<> light_direction;
        float dummy0;
    } vertex_parameters;

    static struct
    {
        mat4_t<> view;
        mat4_t<> model;
        vec2_t<> screen_resolution;
        float t;
    } fragment_parameters;

    renderl_batch_t batch = create_default_batch();

    vertex_parameters.projection = eye.projection;
    vertex_parameters.view = eye.view;
    vertex_parameters.model = model;
    vertex_parameters.light_direction = light_direction;

    fragment_parameters.view = eye.view;
    fragment_parameters.model = model;
    fragment_parameters.screen_resolution = vec2_t<>(window_width, window_height);
    fragment_parameters.t = (float)time_now;

    batch.program = water_program;

    batch.vertex_parameters = &vertex_parameters;
    batch.vertex_parameters_size = sizeof(vertex_parameters);
    batch.fragment_parameters = &fragment_parameters;
    batch.fragment_parameters_size = sizeof(fragment_parameters);

    batch.texture_count = 3;
    batch.textures[0] = &pre_deferred_fbo.textures[0];
    batch.textures[1] = &post_deferred_fbo.textures[0];
    batch.textures[2] = skydome_texture;

    batch.vertex_buffer_count = 2;
    batch.vertex_buffers[0] = &water_mesh.position_buffer;
    batch.vertex_buffers[1] = &water_mesh.normal_buffer;
    batch.index_buffer = &water_mesh.index_buffer;
    batch.index_count = water_mesh.index_count;
    batch.primitive_type = water_mesh.primitive_type;

    batch.use_depth_test = true;
    batch.use_blending = false;
    batch.use_back_face_culling = true;

    renderl_push_batch(batch);
}

void blur(int width, int height, float *in, float *out)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float ar = 0.0f;
            float ag = 0.0f;
            float ab = 0.0f;

            for (int dy = -1; dy <= 1; dy += 2)
            {
                for (int dx = -1; dx <= 1; dx += 2)
                {
                    int rindex = ((x + dx) + (y + dy) * width) * 3;
                    ar += in[rindex + 0];
                    ag += in[rindex + 1];
                    ab += in[rindex + 2];
                }
            }

            int windex = (x + y * width) * 3;
            out[windex + 0] = 0.25f * ar;
            out[windex + 1] = 0.25f * ag;
            out[windex + 2] = 0.25f * ab;
        }
    }
}

void render_system_t::update(float dt)
{
    std::list<item_t> visible_items;
    std::list<light_t> visible_lights;

    entity_list_t players = entity_manager_t::default_manager->entities_possessing_component_type(typeid(player_component_t));
    assert(players.size() == 1);
    int player_entity = *players.begin();
    position_component_t *position = entity_manager_t::default_manager->get_component<position_component_t>(player_entity);
    orientation_component_t *orientation = entity_manager_t::default_manager->get_component<orientation_component_t>(player_entity);
    assert(position);
    assert(orientation);

    renderh_camera_t camera;
    camera.position = position->xyz;
    camera.forward = orientation->rotation.forward();
    camera.right = orientation->rotation.right();
    camera.up = orientation->rotation.up();

    renderm_eye_t camera_eye = renderh_camera_to_eye(camera);
    HAX_camera_eye = &camera_eye;

    extract_visible_stuff(camera, &visible_items, &visible_lights);

    // first, generate all shadow maps
    for (std::list<light_t>::const_iterator iter = visible_lights.begin(); iter != visible_lights.end(); iter++)
    {
        const light_t &light = *iter;
        if (light.shadow_fbo != NULL)
        {
            renderm_eye_t light_eye;
            light_eye.projection = light.light_projection;
            light_eye.view = light.light_view;

            renderl_bind_frame_buffer(light.shadow_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (std::list<item_t>::const_iterator iter = visible_items.begin(); iter != visible_items.end(); iter++)
            {
                const item_t &item = *iter;
                renderh_emit_model_batches(light_eye, item.model_matrix, *item.model);
            }
            renderl_bind_frame_buffer(NULL);

            if (engine_t::instance->input_system.keys['K'])
            {
                rendering_emit_fullscreen_quad_batch(light.shadow_fbo->depth_texture);
                return;
            }
        }
    }

    // do some picking
    if (mouse_x >= 0 && mouse_x < window_width && mouse_y >= 0 && mouse_y < window_height)
    {
        int rendered_models = 0;

        int pick_x = mouse_x;
        int pick_y = window_height - mouse_y - 1;

        glScissor(pick_x, pick_y, 1, 1);
        glEnable(GL_SCISSOR_TEST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (std::list<item_t>::const_iterator iter = visible_items.begin(); iter != visible_items.end(); iter++)
        {
            const item_t &item = *iter;
            renderer_emit_picking_id_batches(camera_eye, item.model_matrix, item.entity, *item.model);
        }
        glDisable(GL_SCISSOR_TEST);
        glScissor(0, 0, window_width, window_height);

        unsigned char res[4];
        glReadPixels(pick_x, pick_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
        int id = 0;
        id += res[3];
        id *= 256;
        id += res[2];
        id *= 256;
        id += res[1];
        id *= 256;
        id += res[0];

        //printf("(%d, %d): %d\n", pick_x, pick_y, id);
    }

    renderl_bind_frame_buffer(&pre_deferred_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    for (std::list<item_t>::const_iterator iter = visible_items.begin(); iter != visible_items.end(); iter++)
    {
        const item_t &item = *iter;
        renderh_emit_model_batches(camera_eye, item.model_matrix, *item.model);
    }
    glDisable(GL_STENCIL_TEST);
    renderl_bind_frame_buffer(NULL);

    renderl_bind_frame_buffer(&post_deferred_fbo);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    for (std::list<light_t>::const_iterator iter = visible_lights.begin(); iter != visible_lights.end(); iter++)
    {
        const light_t &light = *iter;
        const renderl_texture_t *depth_texture;
        if (light.shadow_fbo == NULL)
        {
            depth_texture = NULL;
        }
        else
        {
            depth_texture = &light.shadow_fbo->depth_texture;
        }
        rendering_emit_apply_light_batch(camera_eye, light, pre_deferred_fbo.textures[0], pre_deferred_fbo.textures[1], pre_deferred_fbo.textures[2], pre_deferred_fbo.textures[3], depth_texture);
    }

    // draw skybox
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, 0, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    renderer_emit_draw_skydome_batch(camera_eye);
    glDisable(GL_STENCIL_TEST);

    // end by copying the result to dummy_texture
    glBindTexture(GL_TEXTURE_2D, dummy_texture1.handle);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dummy_texture1.width, dummy_texture1.height);
    glBindTexture(GL_TEXTURE_2D, 0);

    renderl_bind_frame_buffer(NULL);

    renderl_bind_frame_buffer(&render_water_fbo);
    //glClear(GL_COLOR_BUFFER_BIT);

    entity_list_t suns = entity_manager_t::default_manager->entities_possessing_component_type(typeid(sun_component_t));
    assert(suns.size() == 1);
    int sun_entity = *suns.begin();
    directional_light_component_t *light = entity_manager_t::default_manager->get_component<directional_light_component_t>(sun_entity);
    assert(light);

    HAX_light_direction = light->direction;
    
    entity_manager_t::default_manager->iterate_nodes<render_water_surface_component_t, position_component_t>(2, [](render_water_surface_component_t *water_component, position_component_t *pos)
    {
        mat4_t<> model = mat4_t<>::translation(pos->xyz);
        renderer_emit_draw_water_batch(*HAX_camera_eye, HAX_light_direction, model, *water_component->mesh);
    });
    renderl_bind_frame_buffer(NULL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rendering_emit_fullscreen_quad_batch(post_deferred_fbo.textures[0]);
    rendering_emit_fullscreen_quad_batch(render_water_fbo.textures[0]);
    //renderh_emit_ssao_fullscreen_quad_batch(pre_deferred_fbo.depth_texture);

    glClear(GL_DEPTH_BUFFER_BIT);
    entity_manager_t::default_manager->iterate_nodes<sound_source_component_t, position_component_t>(1, [](sound_source_component_t *sound, position_component_t *pos)
    {
        renderer_emit_billboard_batch(*HAX_camera_eye, pos->xyz, speaker_texture);
    });
    glClear(GL_DEPTH_BUFFER_BIT);
    entity_manager_t::default_manager->iterate_nodes<position_component_t, orientation_component_t>(1, [](position_component_t *pos, orientation_component_t *orientation)
    {
        mat4_t<> model_matrix = mat4_t<>::translation(pos->xyz);
        if (orientation)
        {
            model_matrix *= orientation->rotation.rotation_matrix();
        }
        renderer_emit_debug_crosshair_batch(*HAX_camera_eye, model_matrix);
    });
}


/*
    bool hdr = false;
    if (hdr)
    {
        float *data = new float[window_width * window_height * 3];
        glBindFramebuffer(GL_FRAMEBUFFER, post_deferred_fbo.handle);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glReadPixels(0, 0, window_width, window_height, GL_RGB, GL_FLOAT, data);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float sum = 0.0f;
        float max = 0.0f;
        int count = 0;
        //for (int y = 3 * window_height / 8; y < 5 * window_height / 8; y++)
        for (int y = 0; y < window_height; y++)
        {
            //for (int x = 3 * window_width / 8; x < 5 * window_width / 8; x++)
            for (int x = 0; x < window_width; x++)
            {
                int index = (x + y * window_width) * 3;
                float r = data[index + 0];
                float g = data[index + 1];
                float b = data[index + 2];
                float luminance = 0.27f * r + 0.67f * g + 0.06f * b;
                // avoid NaN
                if (luminance == luminance)
                {
                    sum += logf(0.0001f + luminance);
                    if (luminance > max)
                    {
                        max = luminance;
                    }
                    count += 1;
                }
            }
        }
        float lw = expf(sum / count);//(window_width * window_height));
        float a = 0.4f;
        //max = 5000.0f;
        float lwhite = 0.8f * max;
        printf("%f\n", max);
        for (int i = 0; i < window_width * window_height; i++)
        {
            int index = i * 3;
            float r = data[index + 0];
            float g = data[index + 1];
            float b = data[index + 2];
            float luminance = 0.27f * r + 0.67f * g + 0.06f * b;

            // avoid NaN
            if (luminance == luminance)
            {
                float l = a / lw * luminance;
                float ld = (l * (1 + l / (lwhite*lwhite))) / (1 + l);
                
                float scale = 1.5f * ld / luminance;
                r *= scale;
                g *= scale;
                b *= scale;
                data[index + 0] = r;
                data[index + 1] = g;
                data[index + 2] = b;
            }
        }
        float *blur_data1 = new float[window_width * window_height * 3];
        float *blur_data2 = new float[window_width * window_height * 3];
        for (int i = 0; i < window_width * window_height; i++)
        {
            int index = i * 3;
            float r = data[index + 0];
            float g = data[index + 1];
            float b = data[index + 2];
            float luminance = 0.27f * r + 0.67f * g + 0.06f * b;
            float threshold = 0.3f;

            if (luminance != luminance)
            {
                data[index + 0] = 0.0f;
                data[index + 1] = 0.0f;
                data[index + 2] = 0.0f;
            }

            if (luminance > threshold)
            {
                r = (luminance - threshold) * r;
                g = (luminance - threshold) * g;
                b = (luminance - threshold) * b;
            }
            else
            {
                r = 0.0f;
                g = 0.0f;
                b = 0.0f;
            }
            blur_data1[index + 0] = r;
            blur_data1[index + 1] = g;
            blur_data1[index + 2] = b;
        }

        for (int i = 0; i < 64; i++)
        {
            blur(window_width, window_height, blur_data1, blur_data2);
            blur(window_width, window_height, blur_data2, blur_data1);
        }

        for (int i = 0; i < window_width * window_height; i++)
        {
            int index = i * 3;
            data[index + 0] += blur_data1[index + 0];
            data[index + 1] += blur_data1[index + 1];
            data[index + 2] += blur_data1[index + 2];
        }

        delete[] blur_data1;
        delete[] blur_data2;

        //glDrawBuffer(GL_FRONT);
        //glDrawPixels(window_width, window_height, GL_RGBA, GL_FLOAT, data);
        unsigned int temp_tex;
        glGenTextures(1, &temp_tex);
        glBindTexture(GL_TEXTURE_2D, temp_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGB, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);
        renderl_texture_t temp_texture;
        temp_texture.handle = temp_tex;
        temp_texture.width = window_width;
        temp_texture.height = window_height;

        delete[] data;
        renderh_emit_fullscreen_quad_batch(temp_texture);
        glDeleteTextures(1, &temp_tex);
    }
*/

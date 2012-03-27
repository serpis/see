#ifndef _COMPONENTS_HPP
#define _COMPONENTS_HPP

#include <string>

#include "entity_system.hpp"
#include "audiol.hpp"

// dummy component to mark who the player is
class player_component_t : public component_t
{
};

class debug_name_component_t : public component_t
{
public:
    std::string name;
};

class position_component_t : public component_t
{
public:
    vec3_t<> xyz;
};

class orientation_component_t : public component_t
{
public:
    quat_t<> rotation;
};

class sound_source_component_t : public component_t
{
public:
    const audiol_wave_t *wave;
    audiol_voice_t voice;
};

class lens_component_t : public component_t
{
public:
    float fov;
    float aspect;
    float near;
    float far;
};

class render_model_component_t : public component_t
{
public:
    const class renderh_model_t *model;
};

class render_water_surface_component_t : public component_t
{
public:
    const class renderm_mesh_t *mesh;
};

class point_light_component_t : public component_t
{
public:
    vec3_t<> color;
};

class spot_light_component_t : public component_t
{
public:
    vec3_t<> color;
};

class directional_light_component_t : public component_t
{
public:
    vec3_t<> color;
    vec3_t<> direction;
};

// dummy component to mark who the sun is
class sun_component_t : public component_t
{
};

class physics_component_t : public component_t
{
public:
    class btRigidBody *rigid_body;
    bool in_system;
};

class shadow_caster_component_t : public component_t
{
public:
    class renderl_frame_buffer_t *shadow_fbo;
};

void print_component_contents(const component_t *component);

#endif // _COMPONENTS_HPP


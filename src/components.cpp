#include "components.hpp"

static void print_vec3(const char *name, const vec3_t<> &v)
{
    printf("    %s: x: %.2f, y: %.2f, z: %.2f\n", name, v.x, v.y, v.z);
}

static void print_quat(const char *name, const quat_t<> &q)
{
    printf("    %s: x: %.2f, y: %.2f, z: %.2f, w: %.2f\n", name, q.x, q.y, q.z, q.w);
}

static void print_float(const char *name, float f)
{
    printf("    %s: %.2f\n", name, f);
}

void print_component_contents(const component_t *component)
{
    if (const position_component_t *p = dynamic_cast<const position_component_t *>(component))
    {
        puts("  position");
        print_vec3("xyz", p->xyz);
    }
    else if (const orientation_component_t *o = dynamic_cast<const orientation_component_t *>(component))
    {
        puts("  orientation");
        print_quat("rotation", o->rotation);
    }
    else if (const lens_component_t *l = dynamic_cast<const lens_component_t *>(component))
    {
        puts("  lens");
        print_float("fov", l->fov);
        print_float("aspect", l->aspect);
        print_float("near", l->near);
        print_float("far", l->far);
    }
    else if (const render_model_component_t *r = dynamic_cast<const render_model_component_t *>(component))
    {
        puts("  render_model");
        printf("    model: %p\n", r->model);
    }
    else if (const render_water_surface_component_t *w = dynamic_cast<const render_water_surface_component_t *>(component))
    {
        puts("  render_water_surface");
        printf("    mesh: %p\n", w->mesh);
    }
    else if (const point_light_component_t *l = dynamic_cast<const point_light_component_t *>(component))
    {
        puts("  point_light");
        print_vec3("color", l->color);
    }
    else if (const directional_light_component_t *l = dynamic_cast<const directional_light_component_t *>(component))
    {
        puts("  directional_light");
        print_vec3("color", l->color);
        print_vec3("direction", l->direction);
    }
    else if (const player_component_t *p = dynamic_cast<const player_component_t *>(component))
    {
        puts("  player");
    }
    else if (const debug_name_component_t *d = dynamic_cast<const debug_name_component_t *>(component))
    {
        puts("  debug_name");
        printf("    name: %s\n", d->name.c_str());
    }
    else
    {
        printf("  unhandled component: %s\n", typeid(*component).name());
    }
}


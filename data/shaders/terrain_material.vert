uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 xyz_low;
    vec3 xyz_high;
};

attribute vec3 pos;
attribute vec3 normal;

varying vec3 v_normal;
varying vec3 v_fragment_position;
varying vec3 v_world_normal;
varying vec3 v_world_position;
varying vec2 v_lookup;

varying vec3 v_distance_to_edge;

uniform sampler2D tex[8];

float snoise(vec4 v);

float rnoise(int r, vec4 v)
{
    float accum = 0.0;

    float weight = 1.0;
    while (r > 0)
    {
        accum += weight * snoise(v);

        weight *= 0.5;
        v *= 2.0;

        r -= 1;
    }

    return accum;
}

void main()
{
    vec3 world_pos = (model * vec4(pos, 1.0)).xyz;

    vec3 diff = xyz_high - xyz_low;
    vec2 lookup = (world_pos.xz - xyz_low.xz) / diff.xz;
    world_pos.y = xyz_low.y + diff.y * texture2D(tex[3], lookup).r;
    v_distance_to_edge = vec3(1.0);
    v_distance_to_edge[gl_VertexID % 3] = 0.0;
    v_lookup = lookup;

    mat4 vm = view * model;
    mat4 pvm = projection * vm;
    gl_Position = projection * view * vec4(world_pos, 1.0);
    v_normal = (vm * vec4(normal, 0.0)).xyz;
    v_fragment_position = (view * vec4(world_pos, 1.0)).xyz;
    v_world_normal = (model * vec4(normal, 0.0)).xyz;
    v_world_position = world_pos;//(model * vec4(pos, 0.0)).xyz;
}


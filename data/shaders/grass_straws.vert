uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    float time;
};

attribute vec3 pos;
attribute vec3 normal;
attribute vec2 tex_coord;

varying vec2 v_tex_coord;
varying vec3 v_normal;
varying vec3 v_fragment_position;

float snoise(vec4 v);

const vec2 wind_dir = 1.0 * normalize(vec2(1.0, 1.0));

float wind_strength(vec3 p)
{
    float noise = snoise(vec4(0.1 * p.xz - 0.2 * time * wind_dir, 0.0, 0.1 * time));
    return pow(0.5 + 0.5 * noise, 2.0);
}

void main()
{
    mat4 vm = view * model;
    mat4 pvm = projection * vm;

    vec3 world_position = (model * vec4(pos, 1.0)).xyz;
    vec2 world_xz = world_position.xz;

    //vec2 offset = wind_dir * (0.5 + 0.1 * tex_coord.t * snoise(vec4(0.1 * world_xz - 0.1 * 2.0 * time * wind_dir, 0.0, 0.1 * 0.1 * time)));
    vec2 offset = wind_dir * tex_coord.t * max(0.0, wind_strength(world_position));

    gl_Position = pvm * vec4(pos + vec3(offset.x, 0.0, offset.y), 1.0);
    v_tex_coord = tex_coord;
    v_normal = (vm * vec4(normal, 0.0)).xyz;
    v_fragment_position = (vm * vec4(pos, 1.0)).xyz;
}


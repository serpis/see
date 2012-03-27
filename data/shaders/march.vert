uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

attribute vec3 pos;
attribute vec3 normal;
attribute vec2 tex_coord;

varying vec2 v_tex_coord;
varying vec3 v_normal;
varying vec3 v_fragment_position;

varying vec3 v_ray_start;

void main()
{
    mat4 vm = view * model;
    mat4 pvm = projection * vm;
    gl_Position = pvm * vec4(pos, 1.0);
    v_tex_coord = tex_coord;
    v_normal = (vm * vec4(normal, 0.0)).xyz;
    v_fragment_position = (vm * vec4(pos, 1.0)).xyz;

    v_ray_start = pos;
}


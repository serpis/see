uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 light_direction;
};

attribute vec3 pos;
attribute vec3 normal;
attribute vec3 tangent;

varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_fragment_position;
varying vec3 v_world_position;
varying vec3 v_to_light;

void main()
{
    mat4 vm = view * model;
    mat4 pvm = projection * vm;
    gl_Position = pvm * vec4(pos, 1.0);
    v_normal = (vm * vec4(normal, 0.0)).xyz;
    v_tangent = (vm * vec4(tangent, 0.0)).xyz;
    vec4 view_pos = vm * vec4(pos, 1.0);
    v_fragment_position = view_pos.xyz / view_pos.w;//(vm * vec4(pos, 1.0)).xyz;
    v_world_position = (model * vec4(pos, 0.0)).xyz;
    v_to_light = -(view * vec4(light_direction, 0.0)).xyz;
}


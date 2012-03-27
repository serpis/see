uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
};

attribute vec3 pos;

varying vec3 v_fragment_position;

void main()
{
    mat4 pv = projection * view;
    gl_Position = projection * vec4((view * vec4(pos, 0.0)).xyz, 1.0);
    v_fragment_position = pos;
}


uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    vec3 offset;
};

attribute vec2 pos;
attribute vec2 tex_coord;

varying vec2 v_tex_coord;

void main()
{
    const bool perspective = true;
    if (perspective)
    {
        vec4 p = view * vec4(offset, 1.0);
        gl_Position = projection * (p + 0.5 * vec4(pos, 0.0, 0.0));
    }
    else
    {
        vec4 p = projection * view * vec4(offset, 1.0);
        p /= p.w;
        gl_Position = (p + 0.1 * vec4(pos, 0.0, 0.0));
    }
    v_tex_coord = tex_coord;
}


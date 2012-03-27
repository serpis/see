uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
};

attribute vec2 pos;
attribute vec2 tex_coord;

varying vec2 v_tex_coord;

void main()
{
    gl_Position = vec4(pos, 0.0, 1.0);
    v_tex_coord = tex_coord;
}


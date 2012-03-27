uniform vertex_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

attribute vec3 pos;

void main()
{
    mat4 pvm = projection * view * model;
    gl_Position = pvm * vec4(pos, 1.0);
}


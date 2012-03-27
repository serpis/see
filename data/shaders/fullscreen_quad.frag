uniform fragment_uniforms
{
    float dummy;
};

varying vec2 v_tex_coord;

uniform sampler2D tex[8];

void main()
{
    gl_FragColor = texture2D(tex[0], v_tex_coord);
}


uniform fragment_uniforms
{
    float dummy;
};

varying vec2 v_tex_coord;

uniform sampler2D tex[8];

void main()
{
    vec4 c = texture2D(tex[0], v_tex_coord);
    if (c.a < 0.5)
    {
        discard;
    }
    gl_FragColor = vec4(c.rgb, 1.0);

}


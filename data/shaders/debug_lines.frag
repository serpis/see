uniform fragment_uniforms
{
    float dummy;
};

varying vec3 v_color;

void main()
{
    gl_FragColor = vec4(v_color, 1.0);
}


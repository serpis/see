uniform fragment_uniforms
{
    float r, g, b, a;
};

varying vec3 v_color;

void main()
{
    gl_FragColor = vec4(r, g, b, a);
}


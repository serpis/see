uniform fragment_uniforms
{
    float dummy;
};

varying vec2 v_tex_coord;

uniform sampler2D tex[8];

void main()
{
    const int iterations = 10;
    const float max_radius = 10.0 * 1.0 / 640.0;
    const float turns = 5.0;
    float occlusions = 0.0;
    float middle = texture2D(tex[0], v_tex_coord);
    for (int i = 0; i < iterations; i++)
    {
        float progress = float(i) / float(iterations);
        float r = max_radius * progress;
        float t = 2.0 * 3.14159 * progress;

        float dx = r * cos(t);
        float dy = r * sin(t);

        vec2 offset = vec2(dx, dy);

        float sample = texture2D(tex[0], v_tex_coord + offset).r;

        if (sample > middle)
        {
            occlusions += 1.0 - progress * progress;
        }
    }
    //gl_FragColor = vec4(v_tex_coord, 0.0, 1.0);
    gl_FragColor = vec4(vec3(float(occlusions) / float(iterations)), 1.0);
}


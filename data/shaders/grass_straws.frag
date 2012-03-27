uniform fragment_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

varying vec2 v_tex_coord;
varying vec3 v_normal;
varying vec3 v_fragment_position;

uniform sampler2D tex[8];

void main()
{
    vec3 normal = normalize(v_normal);
    vec4 d = texture2D(tex[0], v_tex_coord);
    if (d.a < 0.5)
    {
        discard;
    }
    // silly hack to get around compiler bug
    else
    {
        d = d;
    }
    vec3 specular = vec3(0.0);
    float shininess = 50.0;

    gl_FragData[0] = vec4(v_fragment_position, 1.0);
    gl_FragData[1] = vec4(normal, 1.0);  
    gl_FragData[2] = vec4(d.rgb, 1.0);
    gl_FragData[3] = vec4(specular, shininess);
}


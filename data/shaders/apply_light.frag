uniform fragment_uniforms
{
    mat4 view;
    mat4 view_to_light_view; // when spotlight (1)
    mat4 light_projection; // when spotlight(1)
    int type;
    vec3 light_position; // when pointlight (0) or spotlight (1), this is the position
    vec3 light_direction; // when directional (2), this is the direction the light shines
    vec3 light_color;
};

varying vec2 v_tex_coord;

uniform sampler2D tex[8];

float shadow(vec4 projected, float point_depth)
{
    vec2 sample_base = 0.5 * projected.xy + 0.5;
    //float shadow_depth = texture2D(tex[4], sample_base).r;
    vec2 d = vec2(1.0/512.0);

    float accum = 0.0;
    int dxs[9] = { -1, -1, -1,  0,  0,  0,  1,  1,  1 };
    int dys[9] = { -1,  0,  1, -1,  0,  1, -1,  0,  1 };
    for (int i = 0; i < 9; i++)
    {
        if (texture2D(tex[4], sample_base + d * vec2(dxs[i], dys[i])).r > point_depth * 0.9999)
        {
            accum += 1.0;
        }
    }

    return accum / 9.0;
    //return point_depth * 0.9999 < shadow_depth;
}

void main()
{
    vec3 position = texture2D(tex[0], v_tex_coord).xyz;
    vec3 normal   = texture2D(tex[1], v_tex_coord).xyz;
    vec3 diffuse  = texture2D(tex[2], v_tex_coord).xyz;
    vec3 specular = texture2D(tex[3], v_tex_coord).xyz;
    float shininess = texture2D(tex[3], v_tex_coord).w;

    vec3 view_light_position = (view * vec4(light_position, 1.0)).xyz;

    vec3 to_light;
    float falloff;
    if (type == 0)
    {
        to_light = normalize(view_light_position - position);

        float falloff_r = length(view_light_position - position);
        falloff = exp(-0.1 * falloff_r);
    }
    else if (type == 1)
    {
        to_light = normalize(view_light_position - position);

        float falloff_r = length(view_light_position - position);
        falloff = exp(-0.1 * falloff_r);

        vec4 pos_light_view =  view_to_light_view * vec4(position, 1.0);
        vec4 projected = light_projection * pos_light_view;

        projected /= projected.w;

        float point_depth = projected.z * 0.5 + 0.5;

        float l = length(projected.xy);
        falloff = 1.0 - smoothstep(0.9, 1.0, l);

        if (pos_light_view.z > 0.0)
        {
            discard;
        }

        //float shadow_depth = texture2D(tex[4], 0.5 * projected.xy + 0.5).r;
        falloff *= shadow(projected, point_depth);
        //if (point_depth * 0.9999 - shadow_depth > 0)
        //{
            //discard;
        //}

        
    }
    else
    {
        to_light = (view * vec4(-light_direction, 0.0)).xyz;

        falloff = 1.0f;
    }

    float d = max(0.0, dot(normal, to_light));
    vec3 light_to_point = -to_light;
    vec3 r = reflect(light_to_point, normal);
    float s = pow(max(0.0, dot(r, -normalize(position))), shininess);

    gl_FragColor = vec4(falloff * light_color * (d * diffuse + s * specular), 1.0);
    //gl_FragColor = vec4(type, 0.0, 0.0, 1.0);
}


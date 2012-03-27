uniform fragment_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    float time;
};

varying vec2 v_tex_coord;
varying vec3 v_normal;
varying vec3 v_fragment_position;

varying vec3 v_ray_start;

uniform sampler2D tex[8];

float snoise(vec4 v);

float fnoise(vec4 v)
{
    int iterations = 3;

    float accum = 0.0;

    float scale = 1.0;
    vec4 pos = v;
    for (int i = 0; i < iterations; i++)
    {
        accum += scale * snoise(pos);

        scale /= 2.0;
        pos *= 2.0;
    }

    return 0.5 * accum;
}

/*float distance(vec3 pos)
{
    float limit = 0.02;
    //float offset = 0.03 * smoothstep(-limit, limit, snoise(vec4(4.0 * pos, time)));
    //float offset = 0.05 * snoise(vec4(4.0 * pos, time));
    vec3 n = normalize(pos);
    float offset = 0.1 * fnoise(vec4(4.0 * pos, 0.1 * time));
    return length(pos - offset * normalize(pos)) - 0.5;
}*/

float onoise(vec4 pos)
{
    return 0.5 * snoise(pos) + 0.5;
}

float odistance(vec3 pos)
{
    //return (smoothstep(0.0, 0.5, pos.y)) - 
    //return onoise(vec4(4.0 * pos, 0.0)) / length(pos.xz);
    return onoise(vec4(4.0 * pos, 0.0)) / pos.y;
}

float distance(vec3 pos)
{
    return 2.0 * odistance(pos) - 1.0;
}

vec3 normal(vec3 pos)
{
    float d = 0.001;
    vec3 dx = vec3(  d, 0.0, 0.0);
    vec3 dy = vec3(0.0,   d, 0.0);
    vec3 dz = vec3(0.0, 0.0,   d);
    return normalize(vec3(distance(pos + dx) - distance(pos - dx),
                          distance(pos + dy) - distance(pos - dy),
                          distance(pos + dz) - distance(pos - dz)));
}

bool inside(vec3 pos)
{
    return distance(pos) < 0.5;
}

void main()
{
    mat4 vm = view * model;
    mat4 pvm = projection * vm;
    mat3 vm_3_transpose = transpose(mat3(vm[0].xyz, vm[1].xyz, vm[2].xyz));

    vec3 ray_start = v_ray_start;
    vec3 ray_dir = vm_3_transpose * normalize(v_fragment_position);

    float last_t = 0.0;
    const int linear_iterations = 12;
    const int search_iterations = 8;
    for (int i = 1; i < linear_iterations; i++)
    {
        float t = float(i) / float(linear_iterations);
        vec3 pos = ray_start + t * ray_dir;
        if (inside(pos))
        {
            float lower = last_t;
            float upper = t;
            float best_t;
            for (int j = 0; j < search_iterations; j++)
            {
                float middle = 0.5 * (lower + upper);
                if (inside(ray_start + middle * ray_dir))
                {
                    upper = middle;
                }
                else
                {
                    lower = middle;
                }
            }

            vec3 best_pos = ray_start + (0.5 * (lower + upper)) * ray_dir;
                
            vec3 n = normal(best_pos);
            vec4 p = vm * vec4(best_pos, 1.0);
            p /= p.w;
            vec4 p2 = pvm * vec4(best_pos, 1.0);
            p2.z /= p2.w;
            gl_FragDepth = p2.z * 0.5 + 0.5;

            gl_FragData[0] = vec4(p);  
            gl_FragData[1] = vec4((view * model * vec4(n, 0.0)).xyz, 1.0);  
            gl_FragData[2] = vec4(1.0, 0.0, 1.0, 1.0);
            gl_FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
        else
        {
            last_t = t;
        }
    }

    discard;

    //gl_FragData[0] = vec4(v_fragment_position, 1.0);
    //gl_FragData[1] = vec4(normalize(v_normal), 1.0);  
    //gl_FragData[2] = vec4(diffuse * texture2D(tex[0], v_tex_coord).rgb, 1.0);  
    //gl_FragData[3] = vec4(specular, shininess);

    gl_FragData[0] = vec4(v_fragment_position, 1.0);
    gl_FragData[1] = vec4(normalize(v_normal), 1.0);  
    //gl_FragData[1] = vec4(0.0, 1.0, 0.0, 1.0);  
    //gl_FragData[2] = vec4(1.0, 0.0, 1.0, 1.0);
    gl_FragData[2] = vec4(ray_dir, 1.0);
    gl_FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
}


uniform fragment_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    bool has_ambient_map;
    bool has_diffuse_map;
    bool has_specular_map;
    bool has_normal_map;
    vec2 normal_map_dimensions;
};

varying vec2 v_tex_coord;
varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_fragment_position;

uniform sampler2D tex[8];

void main()
{
    int next_sampler = 0;
    int ambient_map_index;
    int diffuse_map_index;
    int specular_map_index;
    int normal_map_index;
    if (has_ambient_map)
    {
        ambient_map_index = next_sampler++;
    }
    if (has_diffuse_map)
    {
        diffuse_map_index = next_sampler++;
    }
    if (has_specular_map)
    {
        specular_map_index = next_sampler++;
    }
    if (has_normal_map)
    {
        normal_map_index = next_sampler++;
    }

    vec3 normal = normalize(v_normal);
    vec4 d = vec4(1.0);
    if (has_diffuse_map) 
    {
        d = texture2D(tex[diffuse_map_index], v_tex_coord);
        if (d.a < 0.5)
        {
            discard;
        }
        // silly hack to get around compiler bug
        else
        {
            d = d;
        }
    }
    if (has_normal_map) 
    {
        vec3 tangent = normalize(v_tangent);
        vec3 bitangent = cross(normal, tangent);
        mat3 m = mat3(tangent, bitangent, normal);
        vec3 sampled_normal = normalize(vec3(2.0, 2.0, 2.0) * (texture2D(tex[normal_map_index], v_tex_coord).xyz - vec3(0.5, 0.5, 0.5)));
        normal = m * sampled_normal;
    }
    vec3 specular = vec3(0.0);
    float shininess = 50.0;

    gl_FragData[0] = vec4(v_fragment_position, 1.0);
    gl_FragData[1] = vec4(normal, 1.0);  
    gl_FragData[2] = vec4(d.rgb, 1.0);
    gl_FragData[3] = vec4(specular, shininess);
}


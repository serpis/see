uniform fragment_uniforms
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

varying vec3 v_normal;
varying vec3 v_fragment_position;
varying vec3 v_world_normal;
varying vec3 v_world_position;
varying vec2 v_lookup;

varying vec3 v_distance_to_edge;

uniform sampler2D tex[8];

float snoise(vec4 v);

float rnoise(int r, vec4 v)
{
    float accum = 0.0;

    float weight = 1.0;
    while (r > 0)
    {
        accum += weight * snoise(v);

        weight *= 0.5;
        v *= 2.0;

        r -= 1;
    }

    return accum;
}

vec4 get_normal(vec2 p)
{
    //return texture2D(tex[4], p);
    vec2 myTexResolution = vec2(2048.0, 2048.0);

    p = p*myTexResolution + 0.5;

    vec2 i = floor(p);
    vec2 f = p - i;
    f = f*f*f*(f*(f*6.0-15.0)+10.0);
    p = i + f;

    p = (p - 0.5)/myTexResolution;
    return texture2D(tex[4], p);
}

void main()
{
    vec3 world_normal = normalize(v_world_normal);
    //world_normal = 2.0 * (get_normal(v_lookup).xyz - 0.5);
    //world_normal = vec3(0.0, 1.0, 0.0);
    float x_contribution = abs(dot(world_normal, vec3(1.0, 0.0, 0.0)));
    float y_contribution = abs(dot(world_normal, vec3(0.0, 1.0, 0.0)));
    float z_contribution = abs(dot(world_normal, vec3(0.0, 0.0, 1.0)));

    float grass = smoothstep(0.6, 2.5, v_world_position.y + 2.0 * (rnoise(2, vec4(0.05 * v_world_position, 0.0))));

    vec3 x_color = texture2D(tex[1], 0.2*v_world_position.zy).rgb;
    vec3 y_color = mix(texture2D(tex[2], 0.2*v_world_position.xz).rgb, texture2D(tex[0], 0.2*v_world_position.xz).rgb, grass);
    vec3 z_color = texture2D(tex[1], 0.2*v_world_position.xy).rgb;

    float dist = max(max(v_distance_to_edge.x, v_distance_to_edge.y), v_distance_to_edge.z);

    gl_FragData[0] = vec4(v_fragment_position, 1.0);
    gl_FragData[1] = vec4(normalize(v_normal), 1.0);  
    gl_FragData[2] = vec4(diffuse * (x_contribution * x_color + y_contribution * y_color + z_contribution * z_color), 1.0);
    //gl_FragData[2] = vec4(texture2D(tex[4], v_lookup).rgb, 1.0);
    //gl_FragData[2] = vec4(world_normal, 1.0);
    //gl_FragData[2] -= vec4(vec3(smoothstep(0.8, 1.0, dist)), 1.0);
    gl_FragData[3] = vec4(vec3(0.0), 1.0);
}


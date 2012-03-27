// gah this shader is really broken... the coordinate systems are severly confused!


uniform fragment_uniforms
{
    mat4 view;
    mat4 model;
    vec2 screen_resolution;
    float t;
};

varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_fragment_position;
varying vec3 v_world_position;
varying vec3 v_to_light;

uniform sampler2D tex[8];

float snoise(vec4 v);

vec2 snoise2(vec4 p)
{
    return vec2(snoise(p), snoise(p + vec4(2.0, 7.0, 13.0, 0.0)));
}

vec2 perp(float amplitude, float frequency, float time_factor, vec2 p)
{
    //return p;
    return p + amplitude * snoise2(frequency * vec4(p, time_factor * t, 0.0));
}

vec3 water_normal(vec2 surface_position)
{
    vec3 normal = vec3(0.0, 0.0, 1.0);
    float nudge = 1.0;//exp(-0.05*length(v_fragment_position));
    normal.xy += nudge * 0.5 * snoise2(vec4(2.0 * surface_position + perp(0.2, 10.0, 0.06, surface_position), 0.0, 0.3 * t));
    return normalize(normal);
}

void main()
{
    vec3 normal = normalize(v_normal);
    vec3 tangent = normalize(v_tangent);
    vec3 bitangent = cross(normal, tangent);

    mat3 m = mat3(tangent, bitangent, normal);

    vec3 constructed_normal = water_normal((inverse(model) * vec4(v_world_position, 0.0)).xz);

    vec3 from_camera = normalize(v_fragment_position);
    vec3 refracted = refract(from_camera, constructed_normal, 1.1);

    vec2 tc = gl_FragCoord.st / screen_resolution;

    float depth = length(texture2D(tex[0], tc).xyz);
    float travel_distance = abs(length(v_fragment_position.xyz) - depth);

    vec2 offset = 0.1 * travel_distance * (refracted.xy - from_camera.xy);

    float transperancy = 1.0 - pow(1.0 - dot(-from_camera, normal), 2.0);

    vec3 surface_color = vec3(0.1, 0.3, 0.5);
    //vec3 surface_color = vec3(0.0, 0.0, 0.0);
    vec3 falloff_color = vec3(0.0, 0.5, 0.6);
    vec3 bottom_color = texture2D(tex[1], tc + offset).rgb;

    //vec3 bottom_position = texture2D(tex[0], tc + offset).xyz;
    //float caustic = smoothstep(0.0, 1.0, dot(water_normal(bottom_position.xz), vec3(0.0, 1.0, 0.0)));
    //bottom_color += caustic * vec3(1.0);

    vec3 to_light = normalize(v_to_light);
    float d = max(0.0, dot(m * normalize(constructed_normal), to_light));
    vec3 light_to_point = -to_light;
    vec3 r = reflect(light_to_point, m * normalize(constructed_normal));
    float shininess = 30.0;
    float s = pow(max(0.0, dot(r, -normalize(v_fragment_position))), shininess);
    s *= 0.8;
    vec3 light_contribution = vec3(1.0) * (d * surface_color.rgb + s * vec3(1.0));

    //transperancy = 1.0;

    float shore_transperancy = min(1.0, travel_distance);

    gl_FragColor = vec4(shore_transperancy * 
             mix(light_contribution,
                 mix(falloff_color,
                     bottom_color,
                     max(0.0, 1.0 - 0.2 * travel_distance)),
                 transperancy)
             + vec3(s), 1.0);
    gl_FragColor = vec4(mix(bottom_color,
                            mix(light_contribution,
                                mix(falloff_color,
                                    bottom_color,
                                    //max(0.0, 1.0 - 0.2 * travel_distance)),
                                    exp(-0.1 * travel_distance)),
                                transperancy) + vec3(s),
                            shore_transperancy), 1.0);
    //gl_FragColor = vec4(vec3(min(1.0, smoothstep(travel_distance, 0.0, 0.1))), 1.0);
    //gl_FragColor = vec4(vec3(length(texture2D(tex[0], tc).y)), 1.0);
    //gl_FragColor = vec4(vec3(length(texture2D(tex[0], tc).y)), 1.0);
    //gl_FragColor = vec4(vec3(texture2D(tex[0], tc)), 1.0);
    //gl_FragColor = vec4(vec3(length(v_fragment_position.y)), 1.0);
    //gl_FragColor = vec4(vec3(smoothstep(travel_distance, 0.0, 1.0)), 1.0);
    //gl_FragData[1] = vec4(m * normalize(constructed_normal), 1.0);
    //gl_FragData[2] = vec4(0.0, 0.5, 1.0, 1.0);  
    //gl_FragData[3] = vec4(specular, shininess);
    //gl_FragData[3] = vec4(1.0, 1.0, 1.0, 50.0);
}


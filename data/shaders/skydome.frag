uniform fragment_uniforms
{
    float dummy;
};

varying vec3 v_fragment_position;

uniform sampler2D tex[8];

vec3 skydome_lookup(vec3 dir)
{
#define M_PI 3.1415926535897932384626433832795
    float theta = acos(dir.y);
    float phi = atan(dir.z, dir.x);

    float lookup_s = phi / (2.0 * M_PI) + 0.5;
    float lookup_t = 1.0 - theta / M_PI;

    vec2 lookup = vec2(lookup_s, lookup_t);

    //return vec3(lookup, 0.0);
    return texture2D(tex[0], lookup).rgb;
}

void main()
{
    vec3 dir = normalize(v_fragment_position);

    vec3 c = skydome_lookup(dir);

    gl_FragColor = vec4(c, 1.0);
    //gl_FragColor = vec4(dir, 1.0);
    //gl_FragColor = vec4(v_fragment_position, 1.0);
}


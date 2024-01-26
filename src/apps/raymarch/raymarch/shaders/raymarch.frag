#version 420

float distanceSphere(vec3 p, vec3 c, float r)
{
    return length(p - c) - r;
}

float eval(vec3 p)
{
    const vec3 sphere = vec3(0., 0., -3.);
    const float r = 1;

    return distanceSphere(p, sphere, r);
}

vec3 getNormal(vec3 p)
{
    const vec3 eps = vec3(0.001, 0., 0.);

    return normalize(vec3(
        eval(p + eps.xyz) - eval(p - eps.xyz),
        eval(p + eps.yxz) - eval(p - eps.yxz),
        eval(p + eps.yzx) - eval(p - eps.yzx)
    ));
}

out vec4 fragColor;

uniform ivec2 uViewportSize;

const float FAR_PLANE_Z = -50;
const float MAX_HIT_DISTANCE = 0.001;
const int MAX_STEPS = 20;

void main()
{
    const vec3 camera = vec3(0., 0., 5.);

    vec3 fragmentGridPos = vec3(
        (gl_FragCoord.xy / uViewportSize) * 2 - 1,
        0.
    );

    fragmentGridPos.x *= float(uViewportSize.x) / uViewportSize.y;

    vec3 ray = normalize(fragmentGridPos - camera);

    vec3 currentPos = fragmentGridPos;
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        float closest = eval(currentPos);

        if(closest < MAX_HIT_DISTANCE)
        {
            vec3 albedo = vec3(1., 1., 1.);

            float d = max(0., dot(getNormal(currentPos), vec3(-1., 0., 0.5)));

            fragColor = vec4((d * albedo * 0.7 + albedo * 0.3), 1.0);
            return;
        }

        currentPos = currentPos + ray * closest;

        if(currentPos.z < FAR_PLANE_Z)
        {
            return;
        }
    }
}
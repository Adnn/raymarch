#version 420

float sphere(vec3 p, float r)
{
    return length(p) - r;
}

float torus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float smoothMin(float d1, float d2, float k)
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float eval(vec3 p)
{
    const float r = 0.7;
    const vec3 sphere1 = vec3(0.8, 0.8, 0.);
    const vec3 sphere2 = vec3(-0.8, 0.8, 0.);
  
    float s1 = sphere(p - sphere1, r);
    float s2 = sphere(p - sphere2, r);
  
    // Rotate 90° around X
    mat3 torusTransform = mat3(
        1, 0,  0,
        0, 0, -1,
        0, 1,  0
    );
    const vec3 torus1 = vec3(0.0, 0., 0.);
    float t1 = torus(torusTransform * (p - torus1), vec2(0.8, 0.4));
  
    float smoothing = 0.15;
    return smoothMin(s1, smoothMin(s2, t1, smoothing), smoothing);
}

vec3 getNormal(vec3 p)
{
    const vec3 eps = vec3(0.0001, 0., 0.);

    return normalize(vec3(
        eval(p + eps.xyz) - eval(p - eps.xyz),
        eval(p + eps.yxz) - eval(p - eps.yxz),
        eval(p + eps.yzx) - eval(p - eps.yzx)
    ));
}

out vec4 fragColor;

uniform ivec2 uViewportSize;
uniform mat4 uCameraToWorld;

const float FAR_PLANE_Z = -50;
const float MAX_HIT_DISTANCE = 0.001;
const int MAX_STEPS = 100;

void main()
{
    const vec3 camera_cam = vec3(0., 0., 0.);
    const vec3 lightDir = normalize(vec3(1., 0., -.5));

    // Let's place the grid 1 unit in front of the camera (Z = -1)
    vec3 fragmentGridPos_cam = vec3(
        (gl_FragCoord.xy / uViewportSize) * 2 - 1,
        -1.
    );

    // Correct for aspect ratio
    fragmentGridPos_cam.x *= float(uViewportSize.x) / uViewportSize.y;

    vec3 ray = normalize((uCameraToWorld * vec4(fragmentGridPos_cam - camera_cam, 0.)).xyz);

    vec3 currentPos = (uCameraToWorld * vec4(camera_cam, 1.)).xyz;
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        float closest = eval(currentPos);

        if(closest < MAX_HIT_DISTANCE)
        {
            vec3 albedo = vec3(1., 1., 1.);
            float d = max(0., dot(getNormal(currentPos), -lightDir));

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
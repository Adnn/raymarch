#version 420

out vec4 fragColor;

uniform ivec2 uViewportSize;
uniform mat4 uCameraToWorld;

layout(std140) uniform SphereTransforms
{
    mat4 worldToLocal[8];
    uint sphereCount;
};


float smoothMin(float d1, float d2, float k)
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

// A smooth intersection
float smoothMax(float d1, float d2, float k)
{
    float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) + k*h*(1.0-h);
}


//
// 2D Perlin noise
//

//see: https://stegu.github.io/webgl-noise/webdemo/
vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}


//
//	Classic Perlin 3D Noise
//	by Stefan Gustavson
//
//vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
//vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise3(vec3 P)
{
    vec3 Pi0 = floor(P); // Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec3 Pf0 = fract(P); // Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
    vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    vec4 iy = vec4(Pi0.yy, Pi1.yy);
    vec4 iz0 = Pi0.zzzz;
    vec4 iz1 = Pi1.zzzz;

    vec4 ixy = permute(permute(ix) + iy);
    vec4 ixy0 = permute(ixy + iz0);
    vec4 ixy1 = permute(ixy + iz1);

    vec4 gx0 = ixy0 / 7.0;
    vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
    gx0 = fract(gx0);
    vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    vec4 gx1 = ixy1 / 7.0;
    vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
    gx1 = fract(gx1);
    vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
    vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
    vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
    vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
    vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
    vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
    vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
    vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

    vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    vec3 fade_xyz = fade(Pf0);
    vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
    vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}


//
// fBM
//
float sdFbm(vec3 p, float d, float s = 1)
{
   const int octaves = 8;
   for( int i=0; i<octaves; i++ )
   {
       // evaluate new octave
       float n = s * cnoise3(p);

       // add
       n = smoothMax(n, d-0.01*s, 0.3*s);
       d = smoothMin(n, d, 0.3*s);

       // prepare next octave
       p = mat3( 0.00, 1.60, 1.20,
                -1.60, 0.72,-0.96,
                -1.20,-0.96, 1.28 )*p;
       s = 0.5*s;
   }
   return d;
}


float sphere(vec3 p, float r)
{
    return length(p) - r;
}

float torus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float eval_bkp(vec3 p)
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

float eval_b(vec3 p)
{
    const float smoothing = 0.15;
    const float r = 0.8;

    float d0 = 100;
    for(int sIdx = 0; sIdx < sphereCount; ++sIdx)
    {
        vec3 p_local = (worldToLocal[sIdx] * vec4(p, 1.0)).xyz;
        float d = sphere(p_local, r);
        d = max(d, d + 0.02 * cnoise3(p_local * 20.));
        d0 = smoothMin(d0, d, smoothing);
    }
    return d0;
}

float eval(vec3 p)
{
    const float smoothing = 0.15;
    const float r = 1.5;

    float d = sphere(p, r);
    //return smoothMin(d, d + 0.03 * cnoise3(p * 20.), 0.01);
    //return max(d, d + 0.03 * cnoise3(p * 20.));
    return sdFbm(p, d);
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


vec3 correctGamma(vec3 linear)
{
    const float gamma = 2.2;
    return pow(linear, vec3(1.0/gamma));
}

const float MAX_RAY_LENGTH = 50;
const float MAX_HIT_DISTANCE = 0.001;
const int MAX_STEPS = 200;


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

    const vec3 ray = normalize((uCameraToWorld * vec4(fragmentGridPos_cam - camera_cam, 0.)).xyz);
    const vec3 rayOrigin = (uCameraToWorld * vec4(camera_cam, 1.)).xyz;

    float distance = 0;
    for (int i = 0; i < MAX_STEPS; ++i)
    {
        vec3 currentPos = rayOrigin + ray * distance;
        float closest = eval(currentPos);

        if(closest < MAX_HIT_DISTANCE)
        {
            vec3 albedo = vec3(1., 1., 1.);
            float d = max(0., dot(getNormal(currentPos), -lightDir));

            fragColor = vec4(correctGamma(d * albedo * 0.8 + albedo * 0.2), 1.0);
            return;
        }

        distance += closest;
        if(distance > MAX_RAY_LENGTH)
        {
            //fragColor = vec4(0.0, 1.0, 0.0, 1.0);
            return;
        }
    }
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
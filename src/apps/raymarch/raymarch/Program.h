#pragma once

#include <glad/glad.h>

#include <iostream>
#include <vector>


static const char* vertex_shader_text = R"#(
#version 420

in vec2 vPos;

void main()
{
    gl_Position = vec4(vPos, 0.0, 1.0);
}
)#";

static const char* fragment_shader_text = R"#(
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
)#";


void compileShader(GLuint shader)
{
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        std::cerr << "Shader compilation error:\n" << errorLog.data() << std::endl;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        throw std::runtime_error{"Invalid shader code."};
    }
}


struct Program
{
    Program();

    GLuint vertex_shader, fragment_shader, program;

    GLuint viewportSize_location, vpos_location;
};


inline Program::Program()
{
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    compileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    compileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program); // single program

    viewportSize_location = glGetUniformLocation(program, "uViewportSize");

    vpos_location = glGetAttribLocation(program, "vPos");
}

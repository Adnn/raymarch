#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <exception>
#include <iostream>
#include <vector>

#include <cstdlib>
#include <cstdio>

struct Vertex
{
    float x, y;
};

const Vertex quad[4] =
{
    { -1.f, -1.f },
    {  1.f, -1.f },
    { -1.f,  1.f },
    {  1.f,  1.f },
};

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

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

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

int main(void)
{
    GLFWwindow* window;
    GLuint vao, vertex_buffer, vertex_shader, fragment_shader, program;
    GLint vpos_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    window = glfwCreateWindow(640, 480, "Ray marcher", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

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

    const GLuint viewportSize_location = glGetUniformLocation(program, "uViewportSize");

    vpos_location = glGetAttribLocation(program, "vPos");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(quad[0]), (void*) 0);

    while (!glfwWindowShouldClose(window))
    {
        GLint width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glUniform2i(viewportSize_location, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

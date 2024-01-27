#pragma once

#include <build.h>

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>


const std::string getShaderPrefix()
{
    return gRepoFolder + std::string{"/src/apps/raymarch/raymarch/shaders/"};
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


void shaderSourceFromFile(GLuint aShader, const std::string & aFilename)
{
    std::string fullPath = getShaderPrefix() + aFilename;
    if(!std::filesystem::is_regular_file(fullPath))
    {
        throw std::runtime_error{"Not a file: '" + fullPath + "'"};
    }

    std::ifstream file{fullPath};
    std::string code{std::istreambuf_iterator<char>{file}, {}};
    const char * c_code = code.c_str();
    glShaderSource(aShader, 1, &c_code, NULL);
}


struct Program
{
    Program();
    ~Program();

    void compile();

    GLuint vertex_shader, fragment_shader, program;

    GLuint viewportSize_location, vpos_location, cameraTransform_location;
};


inline Program::Program()
{
    compile();
}

Program::~Program()
{
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glDeleteProgram(program);
}

inline void Program::compile()
{
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    shaderSourceFromFile(vertex_shader, "trivial.vert");
    compileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    shaderSourceFromFile(fragment_shader, "raymarch.frag");
    compileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program); // single program

    vpos_location = glGetAttribLocation(program, "vPos");

    viewportSize_location = glGetUniformLocation(program, "uViewportSize");
    cameraTransform_location = glGetUniformLocation(program, "uCameraToWorld");
}
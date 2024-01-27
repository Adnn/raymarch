#pragma once

#include "CameraControl.h"
#include "Program.h"

#include <math/Matrix.h>
#include <math/Transformations.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace math = ad::math;


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


struct Application
{
    Application();

    void update(double aTimepoint);
    void draw(GLFWwindow* window);

    GLuint vao, vertex_buffer;
    Program program;
    CameraControl camera;
};


Application::Application()
{
    // NOTE: OpenGL error checks have been omitted for brevity

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(program.vpos_location);
    glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(quad[0]), (void*) 0);
}


void Application::update(double aTimepoint)
{
}


void Application::draw(GLFWwindow* window)
{
    GLint width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glUniform2i(program.viewportSize_location, width, height);

    math::Matrix<4, 4, float> cameraToWorld =
        math::trans3d::frameToCanonical(camera.mPose.computeTangentFrame());
    glUniformMatrix4fv(program.cameraTransform_location, 1, false, cameraToWorld.data());

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
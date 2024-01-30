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

// IMPORTANT: this must match the size of the worldToLocal array in the shader
const unsigned int gMaxSpheres = 8;

// Returns the inverste transformations for the spheres
// (i.e., the transformation from world space to local space)
std::array<math::AffineMatrix<4, float>, 4> computeTranforms(double aTime)
{
    return {
        math::trans3d::translate(math::Vec<3, float>{1.f, 1.f, 0.f} * std::sin(aTime * 3)).inverse(),
        math::trans3d::translate(math::Vec<3, float>{1.5f, 0.f, 0.f} * std::sin(aTime)).inverse(),
        (math::trans3d::translate(math::Vec<3, float>{0.f, 1.5f, 0.f})
            * math::trans3d::rotateZ(math::Radian{(float)aTime * 1.5f}))
            .inverse(),
        (math::trans3d::translate(math::Vec<3, float>{0.f, -.5f, 0.f})
            * math::trans3d::rotateZ(math::Radian{(float)aTime * 2.f}))
            .inverse(),
    };
}

struct Application
{
    Application();

    void update(double aTimepoint);
    void draw(GLFWwindow* window);

    GLuint vao, vertex_buffer, transform_ubo;
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

    glGenBuffers(1, &transform_ubo);
    // Also binds to the general binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, Program::gSphereTransformBindingLocation, transform_ubo);
    glBufferData(
        GL_UNIFORM_BUFFER,
        sizeof(math::AffineMatrix<4, float>) * gMaxSpheres + sizeof(GLuint),
        nullptr,
        GL_STREAM_DRAW);

    glEnableVertexAttribArray(program.vpos_location);
    glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(quad[0]), (void*) 0);
}


void Application::update(double aTimepoint)
{
    // Update the spheres positions
    auto transforms = computeTranforms(aTimepoint);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(transforms), transforms.data());
    GLuint sphereCount = (GLuint)transforms.size();
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(math::AffineMatrix<4, float>) * gMaxSpheres, sizeof(GLuint), &sphereCount);
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
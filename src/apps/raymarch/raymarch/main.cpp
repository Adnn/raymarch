#include "Program.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


constexpr double gFpsRefreshPeriod = 0.3;

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


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        Program & program = *static_cast<Program *>(glfwGetWindowUserPointer(window));
        try
        {
            program = Program{}; // reloads the files
            std::cout << "Successfully reloaded program." << std::endl;
        }
        catch (std::exception & aException)
        {
            std::cerr << "Cannot reload program:\n" << aException.what() << "\n";
        }
    }
}

int main(void)
{
    GLFWwindow* window;
    GLuint vao, vertex_buffer;

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
    glfwSwapInterval(0);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    Program program;

    glfwSetWindowUserPointer(window, &program);

    glEnableVertexAttribArray(program.vpos_location);
    glVertexAttribPointer(program.vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(quad[0]), (void*) 0);

    double elapsedSinceRefresh = 0;
    unsigned int framesSinceRefresh = 0;

    double timePoint = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        GLint width, height;

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glUniform2i(program.viewportSize_location, width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double now = glfwGetTime();
        elapsedSinceRefresh += now - timePoint;
        ++framesSinceRefresh;
        timePoint = now;

        if(elapsedSinceRefresh >= gFpsRefreshPeriod)
        {
            std::ostringstream oss;
            oss << "Ray marcher (" << framesSinceRefresh/elapsedSinceRefresh << " fps)";
            glfwSetWindowTitle(window, oss.str().c_str());
            elapsedSinceRefresh = 0.;
            framesSinceRefresh = 0;
        }
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

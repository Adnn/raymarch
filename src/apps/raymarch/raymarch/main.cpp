#include "Application.h"
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
        Application & app = *static_cast<Application *>(glfwGetWindowUserPointer(window));
        try
        {
            app.program = Program{}; // reloads the files
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

    Application app;
    glfwSetWindowUserPointer(window, &app);

    double elapsedSinceRefresh = 0;
    unsigned int framesSinceRefresh = 0;

    double timePoint = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        app.draw(window);

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

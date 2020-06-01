#include "Application.h"

int main(char* args)
{
    Application application;
    application.init();
    application.run();

    glfwTerminate();

    return 0;
}
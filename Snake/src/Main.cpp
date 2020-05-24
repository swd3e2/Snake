
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <entt/entt.hpp>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Application.h"

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <LuaBridge\LuaBridge.h>

bool isButtonPressed(int button)
{
    return InputManager::instance()->isKeyPressed(button);
}


int main(char* args)
{
    /*
    // create a Lua state
    lua_State* L = luaL_newstate();

    // load standard libs 
    luaL_openlibs(L);

    luabridge::getGlobalNamespace(L)
        .addFunction("isButtonPressed", isButtonPressed)
        .addFunction("printMessage", printMessage)
    .beginClass<A>("A")
        .addData("x", &A::x)
        .addData("z", &A::z)
        .addFunction("print", &A::print)
    .endClass();

    A* a = new A();
    luabridge::push(L, a);
    lua_setglobal(L, "a");
    */

    Application application;
    application.init();
    application.run();

    glfwTerminate();

    return 0;
}
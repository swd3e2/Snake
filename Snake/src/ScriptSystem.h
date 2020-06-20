#ifndef _SCRIPT_SYSTEM_H
#define _SCRIPT_SYSTEM_H

#include <entt/entt.hpp>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <LuaBridge\LuaBridge.h>
#include "Components.h"
#include "Input/InputManager.h"
#include <iostream>
#include <string>

bool isButtonPressed(int button) {
    return InputManager::instance()->isKeyPressed(button);
}

void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}

class ScriptSystem {
private:
    entt::registry* registry;

    lua_State* L;
public:
    ScriptSystem(entt::registry* registry) {
        this->registry = registry;
        // create a Lua state
        L = luaL_newstate();

        // load standard libs 
        luaL_openlibs(L);

        luabridge::getGlobalNamespace(L)
            .addFunction("isButtonPressed", isButtonPressed)
            .addFunction("printMessage", printMessage)
        .beginClass<glm::vec3>("vec3")
            .addData("x", &glm::vec3::x)
            .addData("y", &glm::vec3::y)
            .addData("z", &glm::vec3::z)
        .endClass();
    }

    void update() {
        registry->view<Script, Transform>().each([&](Script& script, Transform& transform) {
            luabridge::push(L, &transform.translation);
            lua_setglobal(L, "translation");
            luabridge::push(L, &transform.rotation);
            lua_setglobal(L, "rotation");

            int lscript = luaL_dofile(L, script.filename.c_str()); 
            report_errors(L, lscript);
        });
    }
private:
    void report_errors(lua_State* L, int status) {
        if (status != 0) {
            printf("-- %s\n", lua_tostring(L, -1));
            lua_pop(L, 1); // remove error message
        }
    }
};

#endif
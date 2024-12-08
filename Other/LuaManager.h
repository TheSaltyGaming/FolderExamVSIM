// LuaManager.h
#ifndef LUAMANAGER_H
#define LUAMANAGER_H

#include <iostream>
#include <string>

// Include Lua headers
extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}


extern void InstansiateMesh(float minPos, float maxPos, float scale);

class LuaManager {
public:

    LuaManager() {
        // Initialize Lua
        L = luaL_newstate();
        if (L == nullptr) {
            std::cerr << "Failed to create Lua state.\n";
            exit(EXIT_FAILURE);
        }

        // Load Lua standard libraries
        luaL_openlibs(L);

        // Register C++ functions with Lua
        RegisterFunctions();
    }

    ~LuaManager() {
        // Close Lua
        if (L) {
            lua_close(L);
        }
    }

    // Execute a Lua file
    bool DoFile(const std::string& filePath) {
        if (luaL_dofile(L, filePath.c_str()) != LUA_OK) {
            ReportError();
            return false;
        }
        return true;
    }

    // Execute a Lua string
    bool DoString(const std::string& luaCode) {
        if (luaL_dostring(L, luaCode.c_str()) != LUA_OK) {
            ReportError();
            return false;
        }
        return true;
    }


    void CallLuaFunction(const std::string& functionName) {

        lua_getglobal(L, functionName.c_str());
        if (!lua_isfunction(L, -1)) {
            std::cerr << "Lua function '" << functionName << "' not found.\n";
            lua_pop(L, 1);
            return;
        }


        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            ReportError();
        }
    }

private:
    lua_State* L;

    void RegisterFunctions() {
        lua_register(L, "InstansiateMesh", Lua_InstansiateMesh);
    }

    void ReportError() {
        const char* errorMsg = lua_tostring(L, -1);
        std::cerr << "Lua Error: " << (errorMsg ? errorMsg : "Unknown error") << "\n";
        lua_pop(L, 1);
    }


    static int Lua_InstansiateMesh(lua_State* L) {
        if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
            lua_pushstring(L, "Incorrect arguments to 'InstansiateMesh'. Expected three numbers.");
            lua_error(L);
            return 0;
        }

        float minPos = static_cast<float>(lua_tonumber(L, 1));
        float maxPos = static_cast<float>(lua_tonumber(L, 2));
        float scale = static_cast<float>(lua_tonumber(L, 3));

        InstansiateMesh(minPos, maxPos, scale);

        return 0;
    }
};

#endif // LUAMANAGER_H
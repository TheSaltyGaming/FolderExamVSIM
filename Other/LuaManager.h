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

    // Call a Lua function with specific arguments and retrieve results
    int CallLuaFunction(const std::string& functionName, int a, int b) {
        // Push the function onto the stack
        lua_getglobal(L, functionName.c_str());

        if (!lua_isfunction(L, -1)) {
            std::cerr << "Lua function '" << functionName << "' not found.\n";
            lua_pop(L, 1); // Remove non-function value
            return 0;
        }

        // Push arguments
        lua_pushnumber(L, a);
        lua_pushnumber(L, b);

        // Call the function with 2 arguments and 1 result
        if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
            ReportError();
            return 0;
        }

        // Retrieve the result
        if (!lua_isnumber(L, -1)) {
            std::cerr << "Function '" << functionName << "' must return a number.\n";
            lua_pop(L, 1); // Remove the non-number result
            return 0;
        }

        int result = static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1); // Remove the result from the stack
        return result;
    }

    // Simple REPL for Lua
    void RunInterpreter() {
        std::string input;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            if (input == "exit") break;
            if (!DoString(input)) {
                std::cerr << "Error executing command.\n";
            }
        }
    }

private:
    lua_State* L;

    // Register all C++ functions with Lua
    void RegisterFunctions() {
        lua_register(L, "PrintText", PrintText);
        lua_register(L, "cAdd", cAdd);
        lua_register(L, "average", average);
    }

    // Report Lua errors
    void ReportError() {
        const char* errorMsg = lua_tostring(L, -1);
        std::cerr << "Lua Error: " << (errorMsg ? errorMsg : "Unknown error") << "\n";
        lua_pop(L, 1); // Remove error message from stack
    }

    // ------------------ C++ Functions Exposed to Lua ------------------

    // C++ function to print text from Lua
    static int PrintText(lua_State* L) {
        // Check if the first argument is a string
        if (!lua_isstring(L, 1)) {
            lua_pushstring(L, "Incorrect argument to 'PrintText'. Expected a string.");
            lua_error(L);
            return 0;
        }

        // Get the string from Lua
        const char* text = lua_tostring(L, 1);
        std::cout << text << std::endl;

        return 0; // Number of return values
    }

    // C++ addition function callable from Lua
    static int cAdd(lua_State* L) {
        // Ensure there are at least 2 arguments and they are numbers
        if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
            lua_pushstring(L, "Incorrect arguments to 'cAdd'. Expected two numbers.");
            lua_error(L);
            return 0;
        }

        double n1 = lua_tonumber(L, 1);
        double n2 = lua_tonumber(L, 2);
        lua_pushnumber(L, n1 + n2);
        return 1;
    }

    // C++ average function callable from Lua
    static int average(lua_State* L) {
        // Get the number of arguments
        int n = lua_gettop(L);
        if (n == 0) {
            lua_pushstring(L, "No arguments provided to 'average'.");
            lua_error(L);
            return 0;
        }

        double sum = 0.0;
        for (int i = 1; i <= n; ++i) {
            if (!lua_isnumber(L, i)) {
                lua_pushstring(L, "All arguments to 'average' must be numbers.");
                lua_error(L);
                return 0;
            }
            sum += lua_tonumber(L, i);
        }
        lua_pushnumber(L, sum / n);
        lua_pushnumber(L, sum);
        return 2;
    }
};

#endif // LUAMANAGER_H
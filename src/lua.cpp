#include <cassert>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "util.hpp"

static lua_State *state;

void lua_init()
{
    state = luaL_newstate();
    luaL_openlibs(state);
}

void lua_load_file(const char *filename)
{
    int size = file_size(filename);
    char *buf = new char[size];

    slurp(filename, buf, size);
    int error = luaL_loadbuffer(state, buf, size, filename);
    delete[] buf;
    assert(error != LUA_ERRMEM);
    if (error == LUA_ERRSYNTAX)
    {
        printf("lua_load_file(): %s\n", lua_tostring(state, -1));
    }
    else
    {
        int error = lua_pcall(state, 0, 0, 0);
        assert(error != LUA_ERRMEM && error != LUA_ERRERR);
        if (error == LUA_ERRRUN)
        {
            printf("lua_load_file(): %s\n", lua_tostring(state, -1));
        }
    }
}


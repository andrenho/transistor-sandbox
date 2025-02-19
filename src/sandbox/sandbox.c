#include "sandbox.h"

#include <stdlib.h>
#include <stdio.h>

#include <lauxlib.h>
#include <lualib.h>
#include <stb_ds.h>

#include "board.h"

typedef struct ts_Sandbox {
    ts_Board* boards;
} ts_Sandbox;

ts_Sandbox* ts_sandbox_create()
{
    ts_Sandbox* sb = calloc(1, sizeof(ts_Sandbox));
    arrpush(sb->boards, ts_board_create(10, 10));
    return sb;
}

void ts_sandbox_free(ts_Sandbox* sb)
{
    free(sb);
}

ts_Sandbox* ts_sandbox_unserialize(lua_State* L, char* error_buf, size_t error_sz)
{
    if (!lua_istable(L, -1)) {
        snprintf(error_buf, error_sz, "The returned element is not a table");
        return NULL;
    }

    ts_Sandbox* sb = ts_sandbox_create();
    return sb;
}

ts_Sandbox* ts_sandbox_unserialize_from_string(const char* str, char* error_buf, size_t error_sz)
{
    lua_State* L = luaL_newstate();

    luaL_openlibs(L);
    luaL_dostring(L, str);

    if (lua_gettop(L) != 1) {
        snprintf(error_buf, error_sz, "A element was not added to the stack.");
        lua_close(L);
        return NULL;
    }

    ts_Sandbox* sb = ts_sandbox_unserialize(L, error_buf, error_sz);
    lua_close(L);
    return sb;
}

void ts_sandbox_serialize(ts_Sandbox* sb, char* buf, size_t buf_sz)
{
    snprintf(buf, buf_sz, "{}");
}

#include "sandbox.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <lauxlib.h>
#include <lualib.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "board/board.h"
#include "util/serialize.h"

ts_Response ts_sandbox_init(ts_Sandbox* sb)
{
    memset(sb, 0, sizeof(ts_Sandbox));

    arrpush(sb->boards, (ts_Board) {});
    ts_board_init(&sb->boards[0], sb, 10, 10);

    sb->last_error = TS_OK;
    sb->last_error_message[0] = '\0';
    return TS_OK;
}

ts_Response ts_sandbox_finalize(ts_Sandbox** sb)
{
    for (int i = 0; i < arrlen((*sb)->boards); ++i)
        ts_board_finalize(&(*sb)->boards[i]);
    arrfree((*sb)->boards);

    *sb = NULL;

    return TS_OK;
}

int ts_sandbox_serialize(ts_Sandbox const* sb, int vspace, char* buf, size_t buf_sz)
{
    SR_INIT("{");
    SR_CONT("  boards = {");
    for (int i = 0; i < arrlen(sb->boards); ++i)
        SR_CALL(ts_board_serialize, &sb->boards[i], 4);
    SR_CONT("  },");
    SR_FINI("}");
}

ts_Response ts_sandbox_unserialize(ts_Sandbox* sb, lua_State* L)
{
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "The returned element is not a table");

    ts_sandbox_init(sb);

    lua_getfield(L, -1, "boards");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'boards'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        arrpush(sb->boards, (ts_Board) {});
        ts_Response r = ts_board_unserialize(&arrlast(sb->boards), L, sb);
        if (r != TS_OK)
            return r;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return TS_OK;
}

ts_Response ts_sandbox_unserialize_from_string(ts_Sandbox* sb, const char* str)
{
    ts_Response response = TS_OK;

    lua_State* L = luaL_newstate();

    luaL_openlibs(L);
    if (luaL_dostring(L, str)) {
        response = ts_error(sb, TS_DESERIALIZATION_ERROR, "Error reported from Lua: %s", lua_tostring(L, -1));
        goto end;
    }

    if (lua_gettop(L) != 1) {
        response = ts_error(sb, TS_DESERIALIZATION_ERROR, "A element was not added to the stack.");
        goto end;
    }

    response = ts_sandbox_unserialize(sb, L);
    lua_pop(L, 1);

end:
    lua_close(L);
    return response;
}

ts_Response ts_error(ts_Sandbox* sb, ts_Response response, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    sb->last_error = response;
    vsnprintf(sb->last_error_message, sizeof sb->last_error_message, fmt, ap);

    va_end(ap);
    return response;
}

const char* ts_last_error(ts_Sandbox const* sb, ts_Response* response)
{
    if (response)
        *response = sb->last_error;
    return sb->last_error_message;
}

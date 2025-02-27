#include "sandbox.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <lauxlib.h>
#include <lualib.h>

#include <stb_ds.h>

#include "board/board.h"
#include "component/defaultcomponents.h"
#include "util/serialize.h"

//
// initialization
//

static ts_Result ts_sandbox_init_common(ts_Sandbox* sb)
{
    memset(sb, 0, sizeof(ts_Sandbox));
    ts_component_db_init(&sb->component_db, sb);

    sb->last_error = TS_OK;
    sb->last_error_message[0] = '\0';

    ts_simulation_init(&sb->simulation, sb);

    return TS_OK;
}

ts_Result ts_sandbox_init(ts_Sandbox* sb)
{
    ts_sandbox_init_common(sb);

    arrpush(sb->boards, (ts_Board) {});
    ts_board_init(&sb->boards[0], sb, 20, 10);

    ts_add_default_components(&sb->component_db);

    ts_sandbox_start_simulation(sb);
    return TS_OK;
}

ts_Result ts_sandbox_finalize(ts_Sandbox* sb)
{
    ts_sandbox_end_simulation(sb);

    for (int i = 0; i < arrlen(sb->boards); ++i)
        ts_board_finalize(&sb->boards[i]);
    arrfree(sb->boards);

    ts_component_db_finalize(&sb->component_db);

    return TS_OK;
}

//
// simulation
//

ts_Result ts_sandbox_end_simulation(ts_Sandbox* sb)
{
    return ts_simulation_end(&sb->simulation);
}

ts_Result ts_sandbox_start_simulation(ts_Sandbox* sb)
{
    return ts_simulation_start(&sb->simulation);
}


//
// serialization
//

ts_Result ts_sandbox_serialize(ts_Sandbox const* sb, int vspace, FILE* f)
{
    SR_INIT("{");
    SR_CONT("  boards = {");
    for (int i = 0; i < arrlen(sb->boards); ++i)
        SR_CALL(ts_board_serialize, &sb->boards[i], 4);
    SR_CONT("  },");
    SR_CONT("  component_db = {");
    SR_CALL(ts_component_db_serialize, &sb->component_db, 4);
    SR_CONT("  },");
    SR_CONT("}");
    return TS_OK;
}

ts_Result ts_sandbox_unserialize(ts_Sandbox* sb, lua_State* L)
{
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "The returned element is not a table");

    ts_sandbox_init_common(sb);

    // component db

    lua_getfield(L, -1, "component_db");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'component_db'");
    ts_Result r = ts_component_db_unserialize(&sb->component_db, L, sb);
    ts_add_default_components(&sb->component_db);
    if (r != TS_OK)
        return r;
    lua_pop(L, 1);

    // boards

    lua_getfield(L, -1, "boards");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'boards'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        arrpush(sb->boards, (ts_Board) {});
        if ((r = ts_board_unserialize(&arrlast(sb->boards), L, sb)) != TS_OK)
            return r;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return TS_OK;
}

ts_Result ts_sandbox_unserialize_from_string(ts_Sandbox* sb, const char* str)
{
    ts_Result response = TS_OK;

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

ts_Result ts_sandbox_unserialize_from_file(ts_Sandbox* sb, FILE* f)
{
    char* buffer;
    ssize_t bytes_read = getdelim(&buffer, NULL, '\0', f);
    if (bytes_read < 0)
        return TS_SYSTEM_ERROR;
    ts_Result r = ts_sandbox_unserialize_from_string(sb, buffer);
    free(buffer);
    return r;
}

//
// error handling
//

ts_Result ts_error(ts_Sandbox* sb, ts_Result response, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    sb->last_error = response;
    vsnprintf(sb->last_error_message, sizeof sb->last_error_message, fmt, ap);

    va_end(ap);
    return response;
}

const char* ts_last_error(ts_Sandbox const* sb, ts_Result* response)
{
    if (response)
        *response = sb->last_error;
    return sb->last_error_message;
}

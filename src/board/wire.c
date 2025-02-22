#include "wire.h"

#include <lauxlib.h>
#include <string.h>

#include "sandbox/sandbox.h"
#include "util/serialize.h"

int ts_wire_serialize(ts_Wire const* wire, int, char* buf, size_t buf_sz)
{
    return snprintf(buf, buf_sz, "\"%c%c\"", wire->width, wire->layer);
}

ts_Result ts_wire_unserialize(ts_Wire* wire, lua_State* L, ts_Sandbox* sb)
{
    const char* ws = luaL_checkstring(L, -1);
    if (strlen(ws) != 2)
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Incorrect string for wire: '%s'", ws);
    wire->width = ws[0];
    wire->layer = ws[1];
    return TS_OK;
}

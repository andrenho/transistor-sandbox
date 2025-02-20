#include "direction.h"

#include <lauxlib.h>
#include <string.h>

#include "sandbox/sandbox.h"

const char* ts_direction_serialize(ts_Direction dir)
{
    switch (dir) {
        case TS_CENTER: return "center";
        case TS_N:      return "n";
        case TS_S:      return "s";
        case TS_W:      return "w";
        case TS_E:      return "e";
    }
    return "invalid";
}

ts_Response ts_direction_unserialize(ts_Direction* dir, lua_State* L, ts_Sandbox* sb)
{
    const char* s = luaL_checkstring(L, -1);
    if (strcmp(s, "center") == 0)
        *dir = TS_CENTER;
    else if (strcmp(s, "n") == 0)
        *dir = TS_N;
    else if (strcmp(s, "s") == 0)
        *dir = TS_S;
    else if (strcmp(s, "w") == 0)
        *dir = TS_W;
    else if (strcmp(s, "e") == 0)
        *dir = TS_E;
    else
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Direction invalid: '%s'", s);
    return TS_OK;
}

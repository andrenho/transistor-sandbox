#include "board.h"

#include <lauxlib.h>
#include <stb_ds.h>

#include "util/serialize.h"

ts_Response ts_board_init(ts_Board* board, ts_Sandbox* sb, int w, int h)
{
    board->sandbox = sb;
    board->w = w;
    board->h = h;
    board->wires = NULL;
    return TS_OK;
}

ts_Response ts_board_finalize(ts_Board* board)
{
    hmfree(board->wires);
    return TS_OK;
}

ts_Response ts_board_add_wire(ts_Board* board, ts_Position const* pos, ts_Wire const* wire)
{
    hmput(board->wires, ts_pos_hash(pos), *wire);
    return TS_OK;
}

int ts_board_serialize(ts_Board const* board, int vspace, char* buf, size_t buf_sz)
{
    SR_INIT("{");
    SR_CONT("  w = %d,", board->w);
    SR_CONT("  h = %d,", board->h);
    SR_CONT("  wires = {");
    for (int i = 0; i < hmlen(board->wires); ++i) {
        SR_CONT_INLINE("    [%u] = ", board->wires[i].key);
        SR_CALL_INLINE(ts_wire_serialize, &board->wires[i].value);
        SR_ADD_INLINE(",\n");
    }
    SR_CONT("  },");
    SR_FINI("},");
}

ts_Response ts_board_unserialize(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    lua_getfield(L, -1, "w"); board->w = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, -1, "h"); board->h = luaL_checkinteger(L, -1); lua_pop(L, 1);
    return TS_OK;
}
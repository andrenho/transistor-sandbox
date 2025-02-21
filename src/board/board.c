#include "board.h"

#include <lauxlib.h>
#include <stb_ds.h>

#include "util/serialize.h"
#include "component/componentdb.h"

//
// initialization
//

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
    for (int i = 0; i < hmlen(board->components); ++i)
        ts_component_finalize(&board->components[i].value);
    hmfree(board->components);

    hmfree(board->wires);
    return TS_OK;
}

//
// wires
//

ts_Wire* ts_board_wire(ts_Board const* board, ts_Position pos)
{
    int i = hmgeti(((ts_Board *) board)->wires, ts_pos_hash(pos));
    if (i == -1)
        return NULL;
    return &board->wires[i].value;
}

ts_Response ts_board_add_wire(ts_Board* board, ts_Position pos, ts_Wire wire)
{
    if (pos.x >= board->w || pos.y >= board->h)
        return ts_error(board->sandbox, TS_CANNOT_PLACE, "Wire out of bounds");
    hmput(board->wires, ts_pos_hash(pos), wire);
    return TS_OK;
}

ts_Response ts_board_add_wires(ts_Board* board, ts_Position start, ts_Position end, ts_Orientation orientation, ts_Wire wire)
{
    ts_Position pos[300];
    size_t sz = ts_pos_a_to_b(start, end, orientation, pos, 300);
    for (size_t i = 0; i < sz; ++i)
        ts_board_add_wire(board, pos[i], wire);
    if (sz == 0)
        return ts_error(board->sandbox, TS_CANNOT_PLACE, "No wire has been placed");
    return TS_OK;
}

//
// components
//

ts_Response ts_board_add_component(ts_Board* board, const char* name, ts_Position pos, ts_Direction direction)
{
    ts_Component component = {};
    ts_Response r = ts_component_db_init_component(&board->sandbox->component_db, name, &component);
    component.direction = direction;
    if (r != TS_OK)
        return r;
    hmput(board->components, ts_pos_hash(pos), component);
    return TS_OK;
}

ts_Component* ts_board_component(ts_Board const* board, ts_Position pos)
{
    int i = hmgeti(((ts_Board *) board)->components, ts_pos_hash(pos));
    if (i == -1)
        return NULL;
    return &board->components[i].value;
}

//
// serialization
//

int ts_board_serialize(ts_Board const* board, int vspace, char* buf, size_t buf_sz)
{
    SR_INIT("{");
    SR_CONT("  w = %d,", board->w);
    SR_CONT("  h = %d,", board->h);
    SR_CONT("  wires = {");
    for (int i = 0; i < hmlen(board->wires); ++i) {
        char key[30]; ts_pos_serialize(ts_pos_unhash(board->wires[i].key), key, sizeof key);
        SR_CONT_INLINE("    [%s] = ", key);
        SR_CALL_INLINE(ts_wire_serialize, &board->wires[i].value);
        SR_ADD_INLINE(",\n");
    }
    SR_CONT("  },");
    SR_CONT("  components = {");
    for (int i = 0; i < hmlen(board->components); ++i) {
        char key[30]; ts_pos_serialize(ts_pos_unhash(board->components[i].key), key, sizeof key);
        SR_CONT_INLINE("    [%s] = ", key);
        SR_CALL(ts_component_serialize, &board->components[i].value, 4);
        SR_ADD_INLINE(",\n");
    }
    SR_CONT("  },");
    SR_FINI("},");
}

static ts_Response ts_board_unserialize_wires(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    lua_getfield(L, -1, "wires");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'wires'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        ts_Position pos;
        ts_Wire wire;
        ts_Response r;

        lua_pushvalue(L, -2);
        if ((r = ts_pos_unserialize(&pos, L, sb)) != TS_OK)
            return r;
        lua_pop(L, 1);
        if ((r = ts_wire_unserialize(&wire, L, sb) != TS_OK))
            return r;
        hmput(board->wires, ts_pos_hash(pos), wire);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return TS_OK;
}

static ts_Response ts_board_unserialize_components(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    lua_getfield(L, -1, "components");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'components'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        ts_Position  pos;
        ts_Component component;
        ts_Response  r;

        lua_pushvalue(L, -2);
        if ((r = ts_pos_unserialize(&pos, L, sb)) != TS_OK)
            return r;
        lua_pop(L, 1);
        if ((r = ts_component_unserialize(&component, L, sb) != TS_OK))
            return r;
        hmput(board->components, ts_pos_hash(pos), component);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return TS_OK;
}

ts_Response ts_board_unserialize(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    ts_Response r;

    lua_getfield(L, -1, "w"); int w = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, -1, "h"); int h = luaL_checkinteger(L, -1); lua_pop(L, 1);

    ts_board_init(board, sb, w, h);

    if ((r = ts_board_unserialize_wires(board, L, sb)) != TS_OK)
        return r;
    if ((r = ts_board_unserialize_components(board, L, sb)) != TS_OK)
        return r;

    return TS_OK;
}
#include "board.h"

#include <lauxlib.h>
#include <stb_ds.h>

#include "basic/pos_ds.h"
#include "util/serialize.h"
#include "component/componentdb.h"
#include "sandbox/sandbox.h"

ts_HashPosComponentPtr ts_board_component_tiles(ts_Board const* board);

//
// initialization
//

ts_Result ts_board_init(ts_Board* board, ts_Sandbox* sb, int w, int h)
{
    board->sandbox = sb;
    board->w = w;
    board->h = h;
    board->wires = NULL;
    board->components = NULL;
    return TS_OK;
}

ts_Result ts_board_finalize(ts_Board* board)
{
    for (int i = 0; i < hmlen(board->components); ++i) {
        ts_component_finalize(board->components[i].value);
        free(board->components[i].value);
    }
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

ts_Result ts_board_add_wire(ts_Board* board, ts_Position pos, ts_Wire wire)
{
    ts_Result r = TS_OK;
    ts_sandbox_stop_simulation(board->sandbox);

    if (pos.x < board->w && pos.y < board->h)
        hmput(board->wires, ts_pos_hash(pos), wire);
    else
        r = ts_error(board->sandbox, TS_CANNOT_PLACE, "Wire out of bounds");

    ts_sandbox_start_simulation(board->sandbox);
    return r;
}

ts_Result ts_board_add_wires(ts_Board* board, ts_Position start, ts_Position end, ts_Orientation orientation, ts_Wire wire)
{
    ts_sandbox_stop_simulation(board->sandbox);

    ts_Position pos[300];
    size_t sz = ts_pos_a_to_b(start, end, orientation, pos, 300);
    for (size_t i = 0; i < sz; ++i) {
        if (pos[i].x < board->w && pos[i].y < board->h)
            hmput(board->wires, ts_pos_hash(pos[i]), wire);
    }

    ts_sandbox_start_simulation(board->sandbox);
    return TS_OK;
}

//
// components
//

ts_Result ts_board_add_component(ts_Board* board, const char* name, ts_Position pos, ts_Direction direction)
{
    if (pos.dir != TS_CENTER)
        abort();

    ts_sandbox_stop_simulation(board->sandbox);

    // find component
    ts_ComponentDef const* def = ts_component_db_def(&board->sandbox->component_db, name);
    if (def == NULL)
        return ts_error(board->sandbox, TS_COMPONENT_NOT_FOUND, "Component '%s' not found in database.", name);

    // is it inside the board?
    ts_Rect component_rect = ts_component_def_rect(def, pos, direction);
    ts_Rect board_rect = { (ts_Position) { 0, 0, TS_CENTER }, (ts_Position) { board->w - 1, board->h - 1, TS_CENTER } };
    if (!ts_rect_a_inside_b(component_rect, board_rect))
        goto skip;

    // is there another component here? (TODO)
    ts_HashPosComponentPtr other_components = ts_board_component_tiles(board);
    for (int x = component_rect.top_left.x; x <= component_rect.bottom_right.x; ++x) {
        for (int y = component_rect.top_left.y; y <= component_rect.bottom_right.y; ++y) {
            ts_Position p = { x, y, TS_CENTER };
            if (hmgeti(other_components, ts_pos_hash(p)) >= 0) {
                hmfree(other_components);
                goto skip;
            }
        }
    }
    hmfree(other_components);

    if (ts_board_component(board, pos) != NULL)
        goto skip;

    // init component
    ts_Component* component = calloc(1, sizeof(ts_Component));
    ts_Result r = ts_component_init(component, def, direction);
    if (r != TS_OK)
        goto skip;

    // place component
    hmput(board->components, ts_pos_hash(pos), component);  // pointer owns the object
    ts_component_update_pos(component, board, pos);

skip:
    ts_sandbox_start_simulation(board->sandbox);
    return r;
}

ts_Component* ts_board_component(ts_Board const* board, ts_Position pos)
{
    if (pos.dir != TS_CENTER)
        abort();

    ts_Component* ret = NULL;

    int i = hmgeti(((ts_Board *) board)->components, ts_pos_hash(pos));
    if (i >= 0) {
        ret = board->components[i].value;
    } else {
        ts_HashPosComponentPtr comps = ts_board_component_tiles(board);
        int j = hmgeti(comps, ts_pos_hash(pos));
        if (j >= 0)
            ret = comps[j].value;
    }

    return ret;
}

//
// tile
//

ts_Result ts_board_rotate_tile(ts_Board const* board, ts_Position pos)
{
    if (pos.dir != TS_CENTER)
        abort();

    ts_Component* component = ts_board_component(board, pos);
    if (component != NULL && component->def->type == TS_SINGLE_TILE)
        component->direction = ts_direction_rotate_component(component->direction);
    return TS_OK;
}

ts_Result ts_board_clear_tile(ts_Board const* board, ts_Position pos)
{
    if (pos.dir != TS_CENTER)
        abort();

    // clear component
    ts_Component* component = ts_board_component(board, pos);
    if (component) {
        hmdel(((ts_Board *) board)->components, ts_pos_hash(component->position));
        free(component);
    }

    // clear wires
    hmdel(((ts_Board *) board)->wires, ts_pos_hash((ts_Position) { pos.x, pos.y, TS_N }));
    hmdel(((ts_Board *) board)->wires, ts_pos_hash((ts_Position) { pos.x, pos.y, TS_E }));
    hmdel(((ts_Board *) board)->wires, ts_pos_hash((ts_Position) { pos.x, pos.y, TS_S }));
    hmdel(((ts_Board *) board)->wires, ts_pos_hash((ts_Position) { pos.x, pos.y, TS_W }));

    return TS_OK;
}

ts_HashPosComponentPtr ts_board_component_tiles(ts_Board const* board)
{
    ts_HashPosComponentPtr list = NULL;

    for (int i = 0; i < hmlen(board->components); ++i) {
        ts_Component* component = board->components[i].value;
        ts_Rect rect = ts_component_rect(component, ts_pos_unhash(board->components[i].key));
        for (int x = rect.top_left.x; x <= rect.bottom_right.x; ++x) {
            for (int y = rect.top_left.y; y <= rect.bottom_right.y; ++y) {
                ts_Position pos = { x, y, TS_CENTER };
                hmput(list, ts_pos_hash(pos), component);
            }
        }
    }

    return list;
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
        SR_CALL(ts_component_serialize, board->components[i].value, 4);
        SR_ADD_INLINE(",\n");
    }
    SR_CONT("  },");
    SR_FINI("},");
}

static ts_Result ts_board_unserialize_wires(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    lua_getfield(L, -1, "wires");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'wires'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        ts_Position pos;
        ts_Wire wire;
        ts_Result r;

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

static ts_Result ts_board_unserialize_components(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    lua_getfield(L, -1, "components");
    if (!lua_istable(L, -1))
        return ts_error(sb, TS_DESERIALIZATION_ERROR, "Expected a table 'components'");
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        ts_Position  pos;
        ts_Component* component = calloc(1, sizeof(ts_Component));
        ts_Result  r;

        lua_pushvalue(L, -2);
        if ((r = ts_pos_unserialize(&pos, L, sb)) != TS_OK)
            return r;
        lua_pop(L, 1);
        if ((r = ts_component_unserialize(component, L, sb) != TS_OK))
            return r;
        hmput(board->components, ts_pos_hash(pos), component);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return TS_OK;
}

ts_Result ts_board_unserialize(ts_Board* board, lua_State* L, ts_Sandbox* sb)
{
    ts_Result r;

    lua_getfield(L, -1, "w"); int w = luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_getfield(L, -1, "h"); int h = luaL_checkinteger(L, -1); lua_pop(L, 1);

    ts_board_init(board, sb, w, h);

    r = ts_board_unserialize_wires(board, L, sb);
    if (r == TS_OK)
        r = ts_board_unserialize_components(board, L, sb);

    ts_sandbox_start_simulation(board->sandbox);
    return r;
}
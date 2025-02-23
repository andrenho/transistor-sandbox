#include "component.h"

#include <stdlib.h>
#include <string.h>

#include <lauxlib.h>

#include "componentdef.h"
#include "sandbox/sandbox.h"
#include "util/serialize.h"

ts_Result ts_component_init(ts_Component* component, ts_ComponentDef const* def, ts_Direction direction)
{
    memset(component, 0, sizeof(ts_Component));
    component->def = def;
    component->direction = direction;
    component->data = calloc(1, def->data_size);
    component->pins = calloc(def->n_pins, sizeof component->pins[0]);
    if (def->init)
        def->init(component);
    return TS_OK;
}

ts_Result ts_component_finalize(ts_Component* component)
{
    free(component->data);
    free(component->pins);
    return TS_OK;
}

ts_Result ts_component_update_pos(ts_Component* component, ts_Board const* board, ts_Position pos)
{
    component->board = board;
    component->position = pos;
    return TS_OK;
}

ts_Rect ts_component_rect(ts_Component const* component, ts_Position component_pos)
{
    return ts_component_def_rect(component->def, component_pos, component->direction);
}

size_t ts_component_pin_positions(ts_Component const* component, ts_Position component_pos, ts_PinPos* pin_pos, size_t max_pin_pos)
{
    return ts_component_def_pin_positions(component->def, component_pos, component->direction, pin_pos, max_pin_pos);
}

int ts_component_serialize(ts_Component const* component, int vspace, char* buf, size_t buf_sz)
{
    SR_INIT_INLINE("{");
    SR_CONT("  name = '%s',", component->def->key);
    SR_CONT("  direction = '%s',", ts_direction_serialize(component->direction));
    if (component->def->data_size > 0) {
        SR_CONT_INLINE("  data = {");
        for (size_t i = 0; i < component->def->data_size; ++i)
            SR_CONT_INLINE(" 0x%02x,", component->data[i]);
        SR_CONT(" },");
    }
    SR_FINI_INLINE("}");
}

ts_Result ts_component_unserialize(ts_Component* component, lua_State* L, ts_Sandbox* sb)
{
    // direction
    ts_Direction dir;
    lua_getfield(L, -1, "direction"); ts_direction_unserialize(&dir, L, sb); lua_pop(L, 1);

    // setup component
    lua_getfield(L, -1, "name");
    const char* name = luaL_checkstring(L, -1);
    ts_Result r = ts_component_init(component, ts_component_db_def(&sb->component_db, name), dir);
    lua_pop(L, 1);
    if (r != TS_OK)
        return r;

    // update data
    lua_getfield(L, -1, "data");
    if (!lua_isnil(L, -1)) {
        component->data = realloc(component->data, component->def->data_size);
        for (size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_rawgeti(L, -1, i + 1);
            component->data[i] = luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return TS_OK;
}

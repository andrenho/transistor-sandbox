#include "componentdb.h"

#include <string.h>

#include <stb_ds.h>

#include "component.h"
#include "sandbox/sandbox.h"

ts_Result ts_component_db_init(ts_ComponentDB* db, ts_Sandbox* sb)
{
    memset(db, 0, sizeof(ts_ComponentDB));
    db->sandbox = sb;
    return TS_OK;
}

ts_Result ts_component_db_finalize(ts_ComponentDB* db)
{
    shfree(db->items);
    return TS_OK;
}

ts_Result ts_component_db_add_def_from_lua(ts_ComponentDB* db)
{
    lua_State* L = db->sandbox->L;
    ts_Result r = TS_OK;
    int initial_stack = lua_gettop(L);

    if (!lua_istable(L, -1))
        return ts_error(db->sandbox, TS_COMPONENT_DEF_ERROR, "Component definition file should return a table.");

    size_t len = lua_objlen(L, -1);
    if (len == 0) {
        ts_ComponentDef def;
        if ((r = ts_component_def_init_from_lua(&def, db->sandbox)) != TS_OK)
            return r;
        shputs(db->items, def);
    } else for (size_t i = 0; i < len; ++i) {
        lua_rawgeti(L, -1, i + 1);
        ts_ComponentDef def;
        if ((r = ts_component_def_init_from_lua(&def, db->sandbox)) != TS_OK)
            return r;
        shputs(db->items, def);
        lua_pop(L, 1);
    }

end:
    if (lua_gettop(L) > initial_stack)
        lua_pop(L, initial_stack - lua_gettop(L));
    return r;
}

ts_Result ts_component_db_update_simulation(ts_ComponentDB* db, const char* name, SimulateFn sim_fn)
{
    ts_ComponentDef* def = (ts_ComponentDef *) ts_component_db_def(db, name);
    if (def == NULL)
        return ts_error(db->sandbox, TS_COMPONENT_NOT_FOUND, "Component definition '%s' not found in database.", name);
    def->c_simulate = sim_fn;
    return TS_OK;
}

ts_ComponentDef const* ts_component_db_def(ts_ComponentDB const* db, const char* name)
{
    return shgetp_null(((ts_ComponentDB *) db)->items, name);
}

ts_Result ts_component_db_init_component(ts_ComponentDB const* db, const char* name, ts_Component* component)
{
    ts_ComponentDef const* def = ts_component_db_def(db, name);
    if (def == NULL)
        return ts_error(db->sandbox, TS_COMPONENT_NOT_FOUND, "Component '%s' not found in database.", name);
    return ts_component_init(component, def, TS_N);
}

int ts_component_db_serialize(ts_ComponentDB const* db, int vspace, FILE* f)
{
    // TODO - free non-native components
    return TS_OK;
}

ts_Result ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb)
{
    // TODO
    ts_component_db_init(db, sb);
    return TS_OK;
}

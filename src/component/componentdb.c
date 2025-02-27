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

ts_Result ts_component_db_add_def(ts_ComponentDB* db, ts_ComponentDef const* def)
{
    shputs(db->items, *def);
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

#include "componentdb.h"

#include <string.h>

#include <stb_ds.h>

#include "defaultcomponents.h"

ts_Response ts_component_db_init(ts_ComponentDB* db)
{
    memset(db, 0, sizeof(ts_ComponentDB));
    ts_add_default_components(db);
    return TS_OK;
}

ts_Response ts_component_db_finalize(ts_ComponentDB* db)
{
    shfree(db->items);
    return TS_OK;
}

ts_Response ts_component_db_add_def(ts_ComponentDB* db, ts_ComponentDef const* def)
{
    shputs(db->items, *def);
    return TS_OK;
}

int ts_component_db_serialize(ts_ComponentDB const* db, int vspace, char* buf, size_t buf_sz)
{
    // TODO - free non-native components
    return TS_OK;
}

ts_Response ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb)
{
    // TODO
    ts_component_db_init(db);
    return TS_OK;
}

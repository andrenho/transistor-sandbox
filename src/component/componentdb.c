#include "componentdb.h"

void ts_component_db_init(ts_ComponentDB* db)
{

}

void ts_component_db_finalize(ts_ComponentDB* db)
{

}

int ts_component_db_serialize(ts_ComponentDB const* db, int vspace, char* buf, size_t buf_sz)
{
    // TODO
    return TS_OK;
}

ts_Response ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb)
{
    // TODO
    ts_component_db_init(db);
    return TS_OK;
}
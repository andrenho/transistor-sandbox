#ifndef COMPONENTDB_HH
#define COMPONENTDB_HH

#include "util/response.h"
#include "componentdef.h"
#include <lua.h>

typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_ComponentDB {
    struct {
        const char*     key;
        ts_ComponentDef value;
    } *items;
} ts_ComponentDB;

// initialization
ts_Response ts_component_db_init(ts_ComponentDB* db);
ts_Response ts_component_db_finalize(ts_ComponentDB* db);

// db management
ts_Response ts_component_db_add_def(ts_ComponentDB* db, ts_ComponentDef const* def);

// serialization
int         ts_component_db_serialize(ts_ComponentDB const* db, int vspace, char* buf, size_t buf_sz);
ts_Response ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb);

#endif //COMPONENTDB_HH

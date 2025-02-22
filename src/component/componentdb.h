#ifndef COMPONENTDB_HH
#define COMPONENTDB_HH

#include "util/result.h"
#include "componentdef.h"
#include <lua.h>

typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_ComponentDB {
    ts_Sandbox*      sandbox;
    ts_ComponentDef* items;
} ts_ComponentDB;

// initialization
ts_Result ts_component_db_init(ts_ComponentDB* db, ts_Sandbox* sb);
ts_Result ts_component_db_finalize(ts_ComponentDB* db);

// db management
ts_Result ts_component_db_add_def(ts_ComponentDB* db, ts_ComponentDef const* def);

// components
ts_Result ts_component_db_init_component(ts_ComponentDB const* db, const char* name, ts_Component* component);

// serialization
int         ts_component_db_serialize(ts_ComponentDB const* db, int vspace, char* buf, size_t buf_sz);
ts_Result ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb);

#endif //COMPONENTDB_HH

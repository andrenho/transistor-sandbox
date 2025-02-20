#ifndef COMPONENTDB_HH
#define COMPONENTDB_HH

#include "util/response.h"
#include <lua.h>

typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_ComponentDB {
} ts_ComponentDB;

// initialization
void ts_component_db_init(ts_ComponentDB* db);
void ts_component_db_finalize(ts_ComponentDB* db);

// serialization
int         ts_component_db_serialize(ts_ComponentDB const* db, int vspace, char* buf, size_t buf_sz);
ts_Response ts_component_db_unserialize(ts_ComponentDB* db, lua_State* L, ts_Sandbox* sb);

#endif //COMPONENTDB_HH

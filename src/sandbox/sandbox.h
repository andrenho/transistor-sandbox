#ifndef SANDBOX_H
#define SANDBOX_H

#include <stddef.h>
#include <lua.h>

#include "util/response.h"
#include "component/componentdb.h"
#include "simulation/simulation.h"

typedef struct ts_Board ts_Board;

typedef struct ts_Sandbox {
    ts_Board*      boards;
    ts_ComponentDB component_db;
    ts_Simulation  simulation;
    ts_Response    last_error;
    char           last_error_message[2048];
} ts_Sandbox;

// initialization
ts_Response ts_sandbox_init(ts_Sandbox* sb);
ts_Response ts_sandbox_finalize(ts_Sandbox* sb);

// simulation
ts_Response ts_sandbox_stop_simulation(ts_Sandbox* sb);
ts_Response ts_sandbox_start_simulation(ts_Sandbox* sb);

// serialization
int         ts_sandbox_serialize(ts_Sandbox const* sb, int vspace, char* buf, size_t buf_sz);
ts_Response ts_sandbox_unserialize(ts_Sandbox* sb, lua_State* L);
ts_Response ts_sandbox_unserialize_from_string(ts_Sandbox* sb, const char* str);

// error handling
ts_Response ts_error(ts_Sandbox* sb, ts_Response response, const char* fmt, ...) __attribute__((format(printf, 3, 0)));
const char* ts_last_error(ts_Sandbox const* sb, ts_Response* response);

#endif //SANDBOX_H

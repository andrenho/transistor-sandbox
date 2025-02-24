#ifndef SANDBOX_H
#define SANDBOX_H

#include <stddef.h>
#include <lua.h>

#include "util/result.h"
#include "component/componentdb.h"
#include "simulation/simulation.h"

typedef struct ts_Board ts_Board;

typedef struct ts_Sandbox {
    ts_Board*      boards;
    ts_ComponentDB component_db;
    ts_Simulation  simulation;
    ts_Result      last_error;
    char           last_error_message[2048];
} ts_Sandbox;

typedef struct ts_SandboxConfig {
    bool multithreaded;
    bool heavy;
} ts_SandboxConfig;

// initialization
ts_Result ts_sandbox_init(ts_Sandbox* sb, ts_SandboxConfig config);
ts_Result ts_sandbox_finalize(ts_Sandbox* sb);

// simulation
ts_Result ts_sandbox_stop_simulation(ts_Sandbox* sb);
ts_Result ts_sandbox_start_simulation(ts_Sandbox* sb);

// serialization
int         ts_sandbox_serialize(ts_Sandbox const* sb, int vspace, char* buf, size_t buf_sz);
ts_Result ts_sandbox_unserialize(ts_Sandbox* sb, lua_State* L, ts_SandboxConfig config);
ts_Result ts_sandbox_unserialize_from_string(ts_Sandbox* sb, const char* str, ts_SandboxConfig config);

// error handling
ts_Result ts_error(ts_Sandbox* sb, ts_Result response, const char* fmt, ...) __attribute__((format(printf, 3, 0)));
const char* ts_last_error(ts_Sandbox const* sb, ts_Result* response);

#endif //SANDBOX_H

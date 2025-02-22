#include "simulation.h"

#include <stb_ds.h>

#include "compiler/compiler.h"
#include "sandbox/sandbox.h"

static void ts_simulation_finalize(ts_Simulation* sim)
{
    for (int i = 0; i < arrlen(sim->connections); ++i)
        ts_connection_finalize(&sim->connections[i]);
    arrfree(sim->connections);
}

ts_Result ts_simulation_start(ts_Simulation* sim, ts_Sandbox const* sb)
{
    ts_simulation_stop(sim);
    sim->connections = ts_compiler_compile(sb);
    // TODO - start execution thread
    return TS_OK;
}

ts_Result ts_simulation_stop(ts_Simulation* sim)
{
    // TODO - stop execution thread
    ts_simulation_finalize(sim);
    return TS_OK;
}

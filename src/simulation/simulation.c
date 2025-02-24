#include "simulation.h"

#include <stb_ds.h>
#include <sys/time.h>

#include "compiler/compiler.h"
#include "sandbox/sandbox.h"

static void ts_simulation_finalize(ts_Simulation* sim)
{
    for (int i = 0; i < arrlen(sim->connections); ++i)
        ts_connection_finalize(&sim->connections[i]);
    arrfree(sim->connections);
}

static ts_Result ts_simulation_single_step(ts_Simulation* sim)
{
    return TS_OK;
}

ts_Result ts_simulation_start(ts_Simulation* sim, ts_Sandbox const* sb)
{
    ts_simulation_stop(sim);
    sim->connections = ts_compiler_compile(sb);
    if (sim->multithreaded) {
        // TODO - start execution thread
    }
    return TS_OK;
}

ts_Result ts_simulation_stop(ts_Simulation* sim)
{
    if (sim->multithreaded) {
        // TODO - stop execution thread
    }
    ts_simulation_finalize(sim);
    return TS_OK;
}

ts_Result ts_simulation_configure(ts_Simulation* simulation, bool multithreaded, bool heavy)
{
    simulation->multithreaded = multithreaded;
    simulation->heavy = heavy;
    return TS_OK;
}

ts_Result ts_simulation_run(ts_Simulation* sim, size_t run_for_us)
{
    ts_Result r;
    struct timeval t;
    gettimeofday(&t, NULL);

    long end_time = t.tv_sec * 1000000 + t.tv_usec + run_for_us;
    for (;;) {
        if ((r = ts_simulation_single_step(sim)) != TS_OK)
            break;

        gettimeofday(&t, NULL);
        long current_time = t.tv_sec * 1000000 + t.tv_usec + run_for_us;
        if (current_time > end_time)
            break;
    }

    return r;
}

#ifndef SIMULATION_H
#define SIMULATION_H

#include "basic/position.h"
#include "compiler/compiler.h"

typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_Simulation {
    ts_Connection*    connections;
    bool              multithreaded;
    bool              heavy;
    ts_Sandbox const* sandbox;
} ts_Simulation;

ts_Result ts_simulation_start(ts_Simulation* sim, ts_Sandbox const* sb);
ts_Result ts_simulation_stop(ts_Simulation* sim);

ts_Result ts_simulation_pause(ts_Simulation* sim);
ts_Result ts_simulation_unpause(ts_Simulation* sim);

size_t    ts_simulation_wires(ts_Simulation* sim, ts_Position* positions, uint8_t* data, size_t sz);

ts_Result ts_simulation_configure(ts_Simulation* sim, bool multithreaded, bool heavy);
ts_Result ts_simulation_run(ts_Simulation* sim, size_t run_for_us);

#endif //SIMULATION_H

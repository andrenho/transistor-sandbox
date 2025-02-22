#ifndef SIMULATION_H
#define SIMULATION_H

#include "basic/position.h"
#include "compiler/compiler.h"

typedef struct ts_Simulation {
    ts_Connection* connections;
} ts_Simulation;

ts_Result ts_simulation_start(ts_Simulation* simulation, ts_Sandbox const* sb);
ts_Result ts_simulation_stop(ts_Simulation* simulation);

#endif //SIMULATION_H

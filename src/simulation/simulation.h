#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdint.h>

#include "basic/position.h"
#include "board/wire.h"

typedef struct ts_Board ts_Board;
typedef struct ts_Component ts_Component;
typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_Pin {
    ts_Board*     board;
    ts_Component* component;
    ts_Position   pos;
    uint8_t       pin_no;
} ts_Pin;

typedef struct ts_Connection {
    ts_Pin*   pins;
    ts_Wire** wires;
    uint8_t   value;
} ts_Connection;

typedef struct ts_Simulation {
    ts_Connection* connections;
} ts_Simulation;

ts_Result ts_simulation_start(ts_Simulation* simulation, ts_Sandbox const* sb);
ts_Result ts_simulation_stop(ts_Simulation* simulation);

#endif //SIMULATION_H

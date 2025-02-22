#ifndef COMPILER_H
#define COMPILER_H

#include "simulation.h"

typedef struct { ts_Position key; bool value; } PositionSet;

ts_Connection* ts_compiler_compile(ts_Sandbox const* sb);

#endif //COMPILER_H

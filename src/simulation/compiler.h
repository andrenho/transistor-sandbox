#ifndef COMPILER_H
#define COMPILER_H

#include "util/set.h"
#include "simulation.h"

typedef ts_Position*     PositionArray;

ts_Connection* ts_compiler_compile(ts_Sandbox const* sb);

#endif //COMPILER_H

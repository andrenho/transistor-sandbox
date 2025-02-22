#ifndef COMPILER_H
#define COMPILER_H

#include "util/set.h"
#include "simulation/simulation.h"

ts_Connection* ts_compiler_compile(ts_Sandbox const* sb);

#endif //COMPILER_H

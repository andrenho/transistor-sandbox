#ifndef DIRECTION_H
#define DIRECTION_H
#include <lua.h>

#include "util/response.h"

typedef struct ts_Sandbox ts_Sandbox;

typedef enum ts_Direction {
    TS_CENTER, TS_N, TS_S, TS_W, TS_E,
} ts_Direction;

const char*     ts_direction_serialize(ts_Direction dir);
ts_Response     ts_direction_unserialize(ts_Direction* dir, lua_State* L, ts_Sandbox* sb);

#endif //DIRECTION_H

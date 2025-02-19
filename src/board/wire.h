#ifndef WIRE_H
#define WIRE_H

#include <lua.h>

#include "util/response.h"

struct ts_Sandbox;

typedef enum { TS_W1 = '1' } ts_WireWidth;
typedef enum { TS_TOP = 'T', TS_BOTTOM = 'B' } ts_WireLayer;

typedef struct ts_Wire {
    ts_WireWidth width;
    ts_WireLayer layer;
} ts_Wire;

int         ts_wire_serialize(ts_Wire const* wire, int, char* buf, size_t buf_sz);
ts_Response ts_wire_unserialize(ts_Wire* wire, lua_State* L, struct ts_Sandbox* sb);

#endif //WIRE_H

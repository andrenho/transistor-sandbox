#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include <lua.h>

#include "direction.h"
#include "util/response.h"

typedef struct ts_Sandbox ts_Sandbox;

// max position is 16384 (0x3FFF, 14 bits)

typedef uint32_t ts_PositionHash;

typedef struct ts_Position {
    uint16_t     x, y;
    ts_Direction dir;
} ts_Position;

typedef enum { TS_VERTICAL, TS_HORIZONTAL } ts_Orientation;

bool            ts_pos_equals(ts_Position a, ts_Position b);

ts_PositionHash ts_pos_hash(ts_Position pos);
ts_Position     ts_pos_unhash(ts_PositionHash hash);

int             ts_pos_serialize(ts_Position pos, char* buf, size_t buf_sz);
ts_Response     ts_pos_unserialize(ts_Position* pos, lua_State* L, ts_Sandbox* sb);

size_t          ts_pos_a_to_b(ts_Position a, ts_Position b, ts_Orientation orientation, ts_Position* list, size_t list_sz);

#endif //POSITION_H

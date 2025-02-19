#ifndef POSITION_H
#define POSITION_H

#include <stdint.h>

// max position is 16384 (0x3FFF, 14 bits)

typedef uint32_t ts_PositionHash;

typedef enum ts_Direction {
    TS_CENTER, TS_N, TS_S, TS_W, TS_E,
} ts_Direction;

typedef struct ts_Position {
    uint16_t     x, y;
    ts_Direction dir;
} ts_Position;

ts_PositionHash ts_pos_hash(ts_Position pos);
ts_Position     ts_pos_unhash(ts_PositionHash hash);

#endif //POSITION_H

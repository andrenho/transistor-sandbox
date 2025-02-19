#include "position.h"

ts_PositionHash ts_pos_hash(ts_Position pos)
{
    return (pos.y << 18) | (pos.x << 4) | pos.dir;
}

ts_Position ts_pos_unhash(ts_PositionHash hash)
{
    ts_Position pos;
    pos.x = (hash >> 4) & 0x3fff;
    pos.y = hash >> 18;
    pos.dir = hash & 0b111;
    return pos;
}

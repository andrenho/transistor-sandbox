#include "position.h"

#include <assert.h>
#include <lauxlib.h>
#include <string.h>

#include "sandbox/sandbox.h"
#include "util/minmax.h"

bool ts_pos_equals(ts_Position a, ts_Position b)
{
    return ts_pos_hash(a) == ts_pos_hash(b);
}

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

int ts_pos_serialize(ts_Position pos, char* buf, size_t buf_sz)
{
    return snprintf(buf, buf_sz, "{ %d, %d, '%s' }", pos.x, pos.y, ts_direction_serialize(pos.dir));
}

ts_Response ts_pos_unserialize(ts_Position* pos, lua_State* L, ts_Sandbox* sb)
{
    // TODO
}

static size_t ts_pos_a_to_b_horizontal(uint16_t x1, uint16_t x2, uint16_t y, ts_Position* list, size_t list_sz)
{
    assert(x2 >= x1);

    int i = 0;
    list[i++] = (ts_Position) { x1, y, TS_E };
    for (int x = (x1 + 1); x <= (x2 - 1); ++x) {
        list[i++] = (ts_Position) { x, y, TS_W };
        list[i++] = (ts_Position) { x, y, TS_E };
    }
    list[i++] = (ts_Position) { x2, y, TS_W };
    return i;
}

static size_t ts_pos_a_to_b_vertical(uint16_t x, uint16_t y1, uint16_t y2, ts_Position* list, size_t list_sz)
{
    assert(y2 >= y1);

    int i = 0;
    list[i++] = (ts_Position) { x, y1, TS_S };
    for (int y = (y1 + 1); y <= (y2 - 1); ++y) {
        list[i++] = (ts_Position) { x, y, TS_N };
        list[i++] = (ts_Position) { x, y, TS_S };
    }
    list[i++] = (ts_Position) { x, y2, TS_N };
    return i;
}

size_t ts_pos_a_to_b(ts_Position a, ts_Position b, ts_Orientation orientation, ts_Position* list, size_t list_sz)
{
    if (ts_pos_equals(a, b) && list_sz > 0) {
        list[0] = a;
        return 1;
    }

    int sz = 0;
    if (orientation == TS_HORIZONTAL) {
        sz = ts_pos_a_to_b_horizontal(min(a.x, b.x), max(a.x, b.x), a.y, list, list_sz);
        sz += ts_pos_a_to_b_vertical(b.x, min(a.y, b.y), max(a.y, b.y), &list[sz], list_sz - sz);
    } else if (orientation == TS_VERTICAL) {
        sz = ts_pos_a_to_b_vertical(a.x, min(a.y, b.y), max(a.y, b.y), list, list_sz);
        sz += ts_pos_a_to_b_horizontal(min(a.x, b.x), max(a.x, b.x), b.y, &list[sz], list_sz - sz);
    }
    return sz;
}



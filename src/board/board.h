#ifndef BOARD_H
#define BOARD_H

#include "sandbox/sandbox.h"
#include "position.h"
#include "wire.h"

typedef struct ts_Board {
    ts_Sandbox* sandbox;
    int         w, h;
    struct {
        ts_PositionHash key;
        ts_Wire         value;
    }* wires;
} ts_Board;

ts_Response ts_board_init(ts_Board* board, ts_Sandbox* sb, int w, int h);
ts_Response ts_board_finalize(ts_Board* board);

ts_Response ts_board_add_wire(ts_Board* board, ts_Position const* pos, ts_Wire const* wire);

int         ts_board_serialize(ts_Board const* board, int vspace, char* buf, size_t buf_sz);
ts_Response ts_board_unserialize(ts_Board* board, lua_State* L, ts_Sandbox* sb);

#endif //BOARD_H

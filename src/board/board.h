#ifndef BOARD_H
#define BOARD_H

#include "basic/position.h"
#include "wire.h"
#include "component/component.h"

typedef struct ts_Sandbox ts_Sandbox;

typedef struct W__ { ts_PositionHash key; ts_Wire value;       }* ts_HashPosWire;
typedef struct C__ { ts_PositionHash key; ts_Component* value; }* ts_HashPosComponentPtr;

typedef struct ts_Board {
    ts_Sandbox*            sandbox;
    int                    w, h;
    ts_HashPosWire         wires;
    ts_HashPosComponentPtr components;
} ts_Board;

// initialization
ts_Result ts_board_init(ts_Board* board, ts_Sandbox* sb, int w, int h);
ts_Result ts_board_finalize(ts_Board* board);

// wires
ts_Wire*  ts_board_wire(ts_Board const* board, ts_Position pos);  // NULL if not present
ts_Result ts_board_add_wire(ts_Board* board, ts_Position pos, ts_Wire wire);
ts_Result ts_board_add_wires(ts_Board* board, ts_Position start, ts_Position end, ts_Orientation orientation, ts_Wire wire);
ts_Result ts_board_remove_wire(ts_Board* board, ts_Position position);
size_t    ts_board_wires(ts_Board const* board, ts_Position* positions, uint8_t* data, size_t sz);

// components
ts_Result     ts_board_add_component(ts_Board* board, const char* name, ts_Position pos, ts_Direction direction);
ts_Component* ts_board_component(ts_Board const* board, ts_Position pos);     // NULL if not present

// tile
ts_Result     ts_board_rotate_tile(ts_Board const* board, ts_Position pos);
ts_Result     ts_board_clear_tile(ts_Board const* board, ts_Position pos);

// clearing
// TODO

// serialization
int         ts_board_serialize(ts_Board const* board, int vspace, char* buf, size_t buf_sz);
ts_Result ts_board_unserialize(ts_Board* board, lua_State* L, ts_Sandbox* sb);

#endif //BOARD_H

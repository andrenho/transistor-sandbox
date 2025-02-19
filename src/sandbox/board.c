#include "board.h"

#include <stdlib.h>

ts_Board ts_board_create(int w, int h)
{
    return (ts_Board) {
        .w = w,
        .h = h,
    };
}

void ts_board_free(ts_Board* board)
{
    free(board);
}

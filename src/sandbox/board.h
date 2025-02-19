#ifndef BOARD_H
#define BOARD_H

typedef struct ts_Board {
    int w, h;
} ts_Board;

ts_Board ts_board_create(int w, int h);

#endif //BOARD_H

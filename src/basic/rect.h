#ifndef RECT_H
#define RECT_H
#include "position.h"

typedef struct ts_Rect {
    ts_Position top_left;
    ts_Position bottom_right;
} ts_Rect;

#endif //RECT_H

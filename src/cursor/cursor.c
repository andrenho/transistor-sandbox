#include "cursor.h"

#include <stdlib.h>
#include <string.h>

#include "component/componentdb.h"
#include "sandbox/sandbox.h"

ts_Result ts_cursor_init(ts_Cursor* cursor, ts_Board* board)
{
    memset(cursor, 0, sizeof(ts_Cursor));
    cursor->board = board;
    cursor->selected_direction = TS_N;
    cursor->selected_wire = (ts_Wire) { .width = TS_WIRE_1, .layer = TS_LAYER_TOP };
    return TS_OK;
}

ts_Result ts_cursor_finalize(ts_Cursor* cursor)
{
    return TS_OK;
}

ts_Result ts_cursor_set_pointer(ts_Cursor* cursor, ts_Position pos)
{
    cursor->in_bounds = true;
    cursor->pos = pos;

    if (cursor->wire.drawing && !cursor->wire.orientation_defined) {
        int dx = cursor->wire.starting_pos.x - pos.x;
        int dy = cursor->wire.starting_pos.y - pos.y;
        if (dx != 0 || dy != 0) {
            if (abs(dx) > abs(dy))
                cursor->wire.orientation = TS_HORIZONTAL;
            else
                cursor->wire.orientation = TS_VERTICAL;
            cursor->wire.orientation_defined = true;
        }
    }

    if (cursor->erasing)
        ts_board_clear_tile(cursor->board, cursor->pos);

    return TS_OK;
}

ts_Result ts_cursor_set_pointer_out_of_bounds(ts_Cursor* cursor)
{
    cursor->in_bounds = false;
    return TS_OK;
}

ts_Result ts_cursor_click(ts_Cursor* cursor, ts_CursorButton button)
{
    if (cursor->in_bounds) {
        if (button == TS_BUTTON_LEFT) {
            ts_Component* component = ts_board_component(cursor->board, cursor->pos);
            if (component) {
                ts_component_on_click(component);
            } else if (cursor->selected_def) {
                ts_board_add_component(cursor->board, cursor->selected_def->key, cursor->pos, cursor->selected_direction);
            }
        }
    }
    return TS_OK;
}

ts_Result ts_cursor_release(ts_Cursor* cursor, uint8_t button)
{
    return TS_OK;
}

ts_Result ts_cursor_key_press(ts_Cursor* cursor, char key, uint8_t keymod)
{
    if (cursor->in_bounds) {
        if (keymod == 0) {
            switch (key) {
                case 'r': {
                    ts_Component* component = ts_board_component(cursor->board, cursor->pos);
                    if (component && component->def->type == TS_SINGLE_TILE)
                        ts_board_rotate_tile(cursor->board, cursor->pos);
                    else
                        cursor->selected_direction = ts_direction_rotate_component(cursor->selected_direction);
                    break;
                }
                case 'v':
                    ts_board_add_component(cursor->board, "__vcc", cursor->pos, cursor->selected_direction);
                    break;
                case 'b':
                    ts_board_add_component(cursor->board, "__button", cursor->pos, cursor->selected_direction);
                    break;
                case 'l':
                    ts_board_add_component(cursor->board, "__led", cursor->pos, cursor->selected_direction);
                    break;
                case 'p':
                    ts_board_add_component(cursor->board, "__pnp", cursor->pos, cursor->selected_direction);
                    break;
                case 'n':
                    ts_board_add_component(cursor->board, "__npn", cursor->pos, cursor->selected_direction);
                    break;
                case 'x':
                    cursor->erasing = true;
                    break;
                case 'w':
                    cursor->wire.drawing = true;
                    cursor->wire.orientation_defined = false;
                    cursor->wire.orientation = TS_HORIZONTAL;
                    cursor->wire.starting_pos = cursor->pos;
                    break;
                default: break;
            }
        }
    }
    return TS_OK;
}

ts_Result ts_cursor_key_release(ts_Cursor* cursor, char key)
{
    if (key == 'w' && cursor->wire.drawing) {
        if (ts_pos_equals(cursor->pos, cursor->wire.starting_pos) && cursor->pos.dir != TS_CENTER)
            ts_board_add_wire(cursor->board, cursor->pos, cursor->selected_wire);
        else
            ts_board_add_wires(cursor->board, cursor->wire.starting_pos, cursor->pos, cursor->wire.orientation, cursor->selected_wire);
        cursor->wire.drawing = false;
    } else if (key == 'x') {
        cursor->erasing = false;
    }
    return TS_OK;
}

ts_Result ts_cursor_select_component_def(ts_Cursor* cursor, const char* name)
{
    cursor->selected_def = ts_component_db_def(&((ts_Cursor const *)cursor)->board->sandbox->component_db, name);
    return TS_OK;
}
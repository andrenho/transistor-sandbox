#include "componentdef.h"

#include <stdio.h>
#include <stdlib.h>

ts_Rect ts_component_def_rect(ts_ComponentDef const* def, ts_Position component_pos, ts_Direction dir)
{
    int h;

    switch (def->type) {

        case TS_SINGLE_TILE:
            return (ts_Rect) { component_pos, component_pos };

        case TS_IC_DIP:
            h = def->n_pins / 2;
            if (dir == TS_N || dir == TS_S) {
                return (ts_Rect) {
                    (ts_Position) { component_pos.x - 1, component_pos.y - 1, TS_CENTER },
                    (ts_Position) { component_pos.x + def->ic_width, component_pos.y + h, TS_CENTER },
                };
            } else {
                return (ts_Rect) {
                    (ts_Position) { component_pos.x - 1, component_pos.y - 1, TS_CENTER },
                    (ts_Position) { component_pos.x + h, component_pos.y + def->ic_width, TS_CENTER },
                };
            }

        case TS_IC_QUAD:
            h = def->n_pins / 4;
            return (ts_Rect) {
                (ts_Position) { component_pos.x - 1, component_pos.y - 1, TS_CENTER },
                (ts_Position) { component_pos.x + h, component_pos.y + h, TS_CENTER },
            };

        default:
            abort();
    }
}

static ts_Direction single_tile_next_pin(ts_Direction dir)
{
    switch (dir) {
        case TS_N: return TS_W;
        case TS_W: return TS_S;
        case TS_S: return TS_E;
        case TS_E: return TS_N;
        case TS_CENTER:
        default:
            return TS_CENTER;
    }
}

size_t ts_component_def_pin_positions_single_tile(ts_ComponentDef const* def, ts_Position component_pos, ts_Direction direction, ts_PinPos* pin_pos, size_t max_pin_pos)
{
    ts_Direction dir = direction;
    switch (def->n_pins) {
        case 4:
            for (int i = 0; i < 4; ++i) {
                pin_pos[i] = (ts_PinPos) { .pin_no = i, .pos = { component_pos.x, component_pos.y, dir } };
                dir = single_tile_next_pin(dir);
            }
            return 4;
        case 2:
            pin_pos[0] = (ts_PinPos) { .pin_no = 0, .pos = { component_pos.x, component_pos.y, dir } };
            dir = single_tile_next_pin(single_tile_next_pin(dir));
            pin_pos[1] = (ts_PinPos) { .pin_no = 1, .pos = { component_pos.x, component_pos.y, dir } };
            return 2;
        case 1:
            pin_pos[0] = (ts_PinPos) { .pin_no = 0, .pos = { component_pos.x, component_pos.y, dir } };
            return 1;
        default:
            fprintf(stderr, "Single-tile components with %d pins not supported.", def->n_pins);
            abort();
    }
}

size_t ts_component_def_pin_positions_ic_dip(ts_ComponentDef const* def, ts_Position pos, ts_Direction direction, ts_PinPos* pin_pos, size_t max_pin_pos)
{
    int h = def->n_pins / 2;
    int j = 0;
    uint8_t pin_no = 0;

    switch (direction) {
        case TS_N:
            for (int i = 0; i < h; ++i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x - 1, pos.y + i }, .pin_no = pin_no++ };
            for (int i = (h-1); i >= 0; --i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + def->ic_width, pos.y + i }, .pin_no = pin_no++ };
            break;
        case TS_E:
            for (int i = 0; i < h; ++i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y + def->ic_width }, .pin_no = pin_no++ };
            for (int i = (h-1); i >= 0; --i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y - 1 }, .pin_no = pin_no++ };
            break;
        case TS_S:
            for (int i = (h-1); i >= 0; --i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + def->ic_width, pos.y + i }, .pin_no = pin_no++ };
            for (int i = 0; i < h; ++i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x - 1, pos.y + i }, .pin_no = pin_no++ };
            break;
        case TS_W:
            for (int i = (h-1); i >= 0; --i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y - 1 }, .pin_no = pin_no++ };
            for (int i = 0; i < h; ++i)
                pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y + def->ic_width }, .pin_no = pin_no++ };
            break;
        case TS_CENTER:
        default:
            abort();
    }

    return def->n_pins;
}

size_t ts_component_def_pin_positions_ic_quad(ts_ComponentDef const* def, ts_Position pos, ts_Direction direction, ts_PinPos* pin_pos, size_t max_pin_pos)
{
    int h = def->n_pins / 4;
    int j = 0;
    uint8_t pin_no = 0;

    ts_Direction dir[4];
    switch (direction) {
        case TS_N: dir[0] = TS_W; dir[1] = TS_S; dir[2] = TS_E; dir[3] = TS_N; break;
        case TS_E: dir[0] = TS_N; dir[1] = TS_W; dir[2] = TS_S; dir[3] = TS_E; break;
        case TS_S: dir[0] = TS_E; dir[1] = TS_N; dir[2] = TS_W; dir[3] = TS_S; break;
        case TS_W: dir[0] = TS_S; dir[1] = TS_E; dir[2] = TS_N; dir[3] = TS_W; break;
        case TS_CENTER:
        default:
            abort();
    }

    for (size_t k = 0; k < 4; ++k) {
        switch (dir[k]) {
            case TS_W:
                for (int i = 0; i < h; ++i)
                    pin_pos[j++] = (ts_PinPos) { .pos = { pos.x - 1, pos.y + i }, .pin_no = pin_no++ };
                break;
            case TS_S:
                for (int i = 0; i < h; ++i)
                    pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y + h }, .pin_no = pin_no++ };
                break;
            case TS_E:
                for (int i = (h-1); i >= 0; --i)
                    pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + h, pos.y + i }, .pin_no = pin_no++ };
                break;
            case TS_N:
                for (int i = (h-1); i >= 0; --i)
                    pin_pos[j++] = (ts_PinPos) { .pos = { pos.x + i, pos.y - 1 }, .pin_no = pin_no++ };
                break;
            case TS_CENTER:
            default:
                abort();
        }
    }

    return def->n_pins;
}

size_t ts_component_def_pin_positions(ts_ComponentDef const* def, ts_Position component_pos, ts_Direction direction, ts_PinPos* pin_pos, size_t max_pin_pos)
{
    if (max_pin_pos < def->n_pins)
        abort();

    switch (def->type) {
        case TS_SINGLE_TILE:
            return ts_component_def_pin_positions_single_tile(def, component_pos, direction, pin_pos, max_pin_pos);
        case TS_IC_DIP:
            return ts_component_def_pin_positions_ic_dip(def, component_pos, direction, pin_pos, max_pin_pos);
        case TS_IC_QUAD:
            return ts_component_def_pin_positions_ic_quad(def, component_pos, direction, pin_pos, max_pin_pos);
        default:
            abort();
    }
}
#include "transistor-sandbox.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_ds.h"
#include "sandbox/sandbox.h"

//
// initialization
//

ts_Result ts_transistor_init(ts_Transistor* t, ts_TransistorConfig config)
{
    memset(t, 0, sizeof(ts_Transistor));
    ts_sandbox_init(&t->sandbox);
    t->config = config;
    return TS_OK;
}

ts_Result ts_transistor_unserialize(ts_Transistor* t, ts_TransistorConfig config, const char* str)
{
    memset(t, 0, sizeof(ts_Transistor));
    ts_sandbox_unserialize_from_string(&t->sandbox, str);
    t->config = config;
    return TS_OK;
}

ts_Result ts_transistor_finalize(ts_Transistor* t)
{
    ts_sandbox_finalize(&t->sandbox);
    return TS_OK;
}

//
// locking
//

ts_Result ts_transistor_lock(ts_Transistor* t)
{
    if (t->config.multithreaded) {
        // TODO
    }
    return TS_OK;
}

ts_Result ts_transistor_unlock(ts_Transistor* t)
{
    if (t->config.multithreaded) {
        // TODO
    }
    return TS_OK;
}

//
// boards
//

ts_BoardIdx ts_transistor_add_board(ts_Transistor* t, int w, int h)
{
    abort();  // TODO - not implemented yet
}

//
// component db
//

ts_Result ts_transistor_component_db_add(ts_Transistor* t, ts_ComponentDef const* def)
{
    ts_transistor_lock(t);
    ts_Result r = ts_component_db_add_def(&t->sandbox.component_db, def);
    ts_transistor_unlock(t);
    return r;
}

//
// execution
//

ts_Result ts_transistor_run(ts_Transistor* t, size_t run_for_us)
{
    if (!t->config.multithreaded)
        return ts_simulation_run(&t->sandbox.simulation, run_for_us);
    return TS_OK;
}

//
// cursor
//

ts_Result ts_transistor_cursor_set_pointer(ts_Transistor* t, ts_BoardIdx board_idx, ts_Position pos)
{
    ts_transistor_lock(t);
    ts_Result r = ts_cursor_set_pointer(&t->sandbox.boards[board_idx].cursor, pos);
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_set_pointer_out_of_bounds(ts_Transistor* t, ts_BoardIdx board_idx)
{
    ts_transistor_lock(t);
    ts_Result r = ts_cursor_set_pointer_out_of_bounds(&t->sandbox.boards[board_idx].cursor);
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_click(ts_Transistor* t, ts_BoardIdx board_idx, ts_CursorButton button)
{
    ts_transistor_lock(t);
    ts_Result r = ts_cursor_click(&t->sandbox.boards[board_idx].cursor, button);
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_release(ts_Transistor* t, uint8_t button)
{
    ts_Result r = TS_OK;
    ts_transistor_lock(t);
    for (int i = 0; i < arrlen(t->sandbox.boards); ++i) {
        r = ts_cursor_release(&t->sandbox.boards[i].cursor, button);
        if (r != TS_OK)
            break;
    }
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_key_press(ts_Transistor* t, ts_BoardIdx board_idx, char key, uint8_t keymod)
{
    ts_transistor_lock(t);
    ts_Result r = ts_cursor_key_press(&t->sandbox.boards[board_idx].cursor, key, keymod);
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_key_release(ts_Transistor* t, char key)
{
    ts_Result r = TS_OK;
    ts_transistor_lock(t);
    for (int i = 0; i < arrlen(t->sandbox.boards); ++i) {
        r = ts_cursor_key_release(&t->sandbox.boards[i].cursor, key);
        if (r != TS_OK)
            break;
    }
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_cursor_select_component_def(ts_Transistor* t, const char* name)
{
    ts_Result r = TS_OK;
    ts_transistor_lock(t);
    for (int i = 0; i < arrlen(t->sandbox.boards); ++i) {
        r = ts_cursor_select_component_def(&t->sandbox.boards[i].cursor, name);
        if (r != TS_OK)
            break;
    }
    ts_transistor_unlock(t);
    return r;
}

//
// errors
//

ts_Result ts_transistor_last_error(ts_Transistor* t, char* err_buf, size_t err_buf_sz)
{
    ts_transistor_lock(t);
    ts_Result r;
    const char* err = ts_last_error(&t->sandbox, &r);
    snprintf(err_buf, err_buf_sz, "%s", err);
    ts_transistor_unlock(t);
    return r;
}

//
// snapshots
//

ts_Result ts_transistor_take_snapshot(ts_Transistor const* t, ts_TransistorSnapshot* snap)
{
    snap->boards = calloc(arrlen(t->sandbox.boards), sizeof(ts_BoardSnapshot));
    snap->n_boards = arrlen(t->sandbox.boards);
    for (int i = 0; i < arrlen(t->sandbox.boards); ++i) {

        // board
        ts_Board* board = &t->sandbox.boards[i];
        snap->boards[i] = (ts_BoardSnapshot) {
            .w = board->w,
            .h = board->h,
            .n_components = phlen(board->components),
            .components = calloc(phlen(board->components), sizeof(ts_ComponentSnapshot)),
            .n_wires = phlen(board->wires),
            .wires = calloc(phlen(board->wires), sizeof(ts_WireSnapshot)),
        };

        // components
        for (int j = 0; j < phlen(board->components); ++j) {
            ts_Component* component = board->components[j].value;
            snap->boards[i].components[j] = (ts_ComponentSnapshot) {
                .key = strdup(component->def->key),
                .pos = ts_pos_unhash(board->components[j].key),
                .type = component->def->type,
                .ic_width = component->def->ic_width,
                .n_pins = component->def->n_pins,
                .pins = calloc(component->def->n_pins, sizeof(ts_PinSnapshot)),
                .extra_data = component->def->extra,
            };

            // pins
            for (int k = 0; k < component->def->n_pins; ++k) {
                ts_PinDef* pin = &component->def->pins[k];
                snap->boards[i].components[j].pins[k] = (ts_PinSnapshot) {
                    .name = strdup(pin->name),
                    .type = pin->type,
                };
            }
        }

        // wires
        const int max = phlen(board->wires);
        ts_Position positions[max];
        uint8_t values[max];
        size_t sz = ts_simulation_wires(&t->sandbox.simulation, positions, values, max);
        int k = 0;
        for (size_t j = 0; j < sz; ++j) {
            ts_Wire* wire = ts_board_wire(board, positions[j]);
            if (wire) {
                snap->boards[i].wires[k++] = (ts_WireSnapshot) {
                    .pos = positions[j],
                    .layer = wire->layer,
                    .width = wire->width,
                    .value = values[j],
                };
            }
        }
        snap->boards[i].n_wires = k;
    }

    return TS_OK;
}

ts_Result ts_snapshot_finalize(ts_TransistorSnapshot* snap)
{
    for (size_t i = 0; i < snap->n_boards; ++i) {
        for (size_t j = 0; j < snap->boards[i].n_components; ++j) {
            for (size_t k = 0; k < snap->boards[i].components[j].n_pins; ++k)
                free(snap->boards[i].components[j].pins[k].name);
            free(snap->boards[i].components[j].pins);
            free(snap->boards[i].components[j].key);
        }
        free(snap->boards[i].components);
        free(snap->boards[i].wires);
    }
    free(snap->boards);
    return TS_OK;
}

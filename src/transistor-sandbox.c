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

ts_Result ts_transistor_take_snapshot(ts_Transistor const* t, ts_TransistorSnapshot* snap) {}
ts_Result ts_snapshot_finalize(ts_TransistorSnapshot* snap) {}

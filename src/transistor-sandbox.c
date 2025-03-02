#include "transistor-sandbox.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#include <lua.h>
#include <lauxlib.h>

#include "stb_ds.h"
#include "sandbox/sandbox.h"

const char* ts_transistor_version(int* major, int* minor, int* patch)
{
    if (major)
        *major = PROJECT_VERSION_MAJOR;
    if (minor)
        *minor = PROJECT_VERSION_MINOR;
    if (patch)
        *patch = PROJECT_VERSION_PATCH;
    return PROJECT_VERSION;
}

//
// execution thread
//

static void* thread_run(void* ptr_t)
{
    ts_Transistor* t = ptr_t;

    while (t->thread_running) {

        // pause thread
        pthread_mutex_lock(&t->lock);
        while (t->thread_paused)
            pthread_cond_wait(&t->cond, &t->lock);

        // execute
        ts_simulation_single_step(&t->sandbox.simulation);

        // unpause
        pthread_mutex_unlock(&t->lock);

        // give the CPU a break
        if (!t->config.heavy)
            sched_yield();
    }

    return NULL;
}

static void thread_init(ts_Transistor* t)
{
    if (t->config.multithreaded) {
        t->thread_running = true;
        pthread_mutex_init(&t->lock, NULL);
        pthread_cond_init(&t->cond, NULL);
        pthread_create(&t->thread, NULL, thread_run, t);
    }
}

//
// initialization
//


ts_Result ts_transistor_init(ts_Transistor* t, ts_TransistorConfig config)
{
    memset(t, 0, sizeof(ts_Transistor));
    ts_sandbox_init(&t->sandbox);
    t->config = config;
    thread_init(t);
    return TS_OK;
}

ts_Result ts_transistor_unserialize(ts_Transistor* t, ts_TransistorConfig config, const char* str)
{
    memset(t, 0, sizeof(ts_Transistor));
    ts_Result r = ts_sandbox_unserialize_from_string(&t->sandbox, str);
    if (r != 0)
        return r;
    t->config = config;
    thread_init(t);
    return TS_OK;
}

ts_Result ts_transistor_unserialize_from_file(ts_Transistor* t, ts_TransistorConfig config, FILE* f)
{
    char* buffer;
    ssize_t bytes_read = getdelim(&buffer, NULL, '\0', f);
    if (bytes_read < 0)
        return TS_SYSTEM_ERROR;
    ts_Result r = ts_transistor_unserialize(t, config, buffer);
    free(buffer);
    return r;
}

ts_Result ts_transistor_finalize(ts_Transistor* t)
{
    if (t->config.multithreaded) {
        t->thread_running = false;
        pthread_join(t->thread, NULL);
    }
    ts_sandbox_finalize(&t->sandbox);
    return TS_OK;
}

//
// serialization
//

ts_Result ts_transistor_serialize_to_file(ts_Transistor const* t, FILE* f)
{
    return ts_sandbox_serialize(&t->sandbox, 0, f);
}

//
// locking
//

ts_Result ts_transistor_lock(ts_Transistor* t)
{
    if (t->config.multithreaded && !t->thread_paused) {
        pthread_mutex_lock(&t->lock);
        t->thread_paused = true;
    }
    return TS_OK;
}

ts_Result ts_transistor_unlock(ts_Transistor* t)
{
    if (t->config.multithreaded && t->thread_paused) {
        t->thread_paused = false;
        pthread_cond_signal(&t->cond);
        pthread_mutex_unlock(&t->lock);
    }
    return TS_OK;
}

//
// boards
//

[[noreturn]] ts_BoardIdx ts_transistor_add_board(ts_Transistor* t, int w, int h)
{
    abort();  // TODO - not implemented yet
}

//
// component db
//

ts_Result ts_transistor_component_db_add_from_lua(ts_Transistor* t, const char* lua_code)
{
    ts_Result r;
    ts_transistor_lock(t);
    r = ts_component_db_add_def_from_lua(&t->sandbox.component_db, lua_code);
    ts_transistor_unlock(t);
    return r;
}

ts_Result ts_transistor_component_db_native_simulation(ts_Transistor* t, const char* name, SimulateFn sim_fn)
{
    ts_transistor_lock(t);
    ts_component_db_update_simulation(&t->sandbox.component_db, name, sim_fn);
    ts_transistor_unlock(t);
    return TS_OK;
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
    ts_Result r;
    ts_transistor_lock(t);
    if (pos.x < 0 || pos.y < 0 || pos.x >= t->sandbox.boards[board_idx].w || pos.y >= t->sandbox.boards[board_idx].h)
        r = ts_cursor_set_pointer_out_of_bounds(&t->sandbox.boards[board_idx].cursor);
    else
        r = ts_cursor_set_pointer(&t->sandbox.boards[board_idx].cursor, pos);
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

static void add_component_def(ts_ComponentDef const* def, ts_Position pos, ts_Direction dir, bool cursor, ts_ComponentSnapshot* dest)
{
    *dest = (ts_ComponentSnapshot) {
        .key = strdup(def->key),
        .pos = pos,
        .direction = dir,
        .type = def->type,
        .ic_width = def->ic_width,
        .n_pins = def->n_pins,
        .pins = calloc(def->n_pins, sizeof(ts_PinSnapshot)),
        .cursor = cursor,
    };

    // pins
    for (int k = 0; k < def->n_pins; ++k) {
        ts_PinDef* pin = &def->pins[k];
        dest->pins[k] = (ts_PinSnapshot) {
            .name = strdup(pin->name),
            .type = pin->direction,
        };
    }
}

ts_Result ts_transistor_take_snapshot(ts_Transistor const* t, ts_TransistorSnapshot* snap)
{
    const size_t MAX_WIRE_CURSOR = 2000;

    snap->boards = calloc(arrlen(t->sandbox.boards), sizeof(ts_BoardSnapshot));
    snap->n_boards = arrlen(t->sandbox.boards);
    for (int i = 0; i < arrlen(t->sandbox.boards); ++i) {

        ts_Board* board = &t->sandbox.boards[i];

        // get wire cursor positions
        size_t n_wire_cursor = 0;
        ts_Position wire_cursor[MAX_WIRE_CURSOR];
        if (board->cursor.wire.drawing)
            n_wire_cursor = ts_pos_a_to_b(board->cursor.wire.starting_pos, board->cursor.pos, board->cursor.wire.orientation, wire_cursor, MAX_WIRE_CURSOR);

        // board
        size_t n_components = phlen(board->components) + (board->cursor.selected_def ? 1 : 0);
        size_t n_wires = phlen(board->wires) + n_wire_cursor;
        snap->boards[i] = (ts_BoardSnapshot) {
            .w = board->w,
            .h = board->h,
            .n_components = n_components,
            .components = calloc(n_components, sizeof(ts_ComponentSnapshot)),
            .n_wires = n_wires,
            .wires = calloc(n_wires, sizeof(ts_WireSnapshot)),
        };

        // components
        int j;
        for (j = 0; j < phlen(board->components); ++j) {
            ts_Component* component = board->components[j].value;
            add_component_def(component->def, ts_pos_unhash(board->components[j].key), component->direction, false, &snap->boards[i].components[j]);
        }
        if (board->cursor.selected_def)
            add_component_def(board->cursor.selected_def, board->cursor.pos, board->cursor.selected_direction, true, &snap->boards[i].components[j]);

        // wires
        const int max = phlen(board->wires);
        ts_Position positions[max];
        uint8_t values[max];
        size_t sz = ts_simulation_wires(&t->sandbox.simulation, positions, values, max);
        int k = 0;
        for (size_t m = 0; m < sz; ++m) {
            ts_Wire* wire = ts_board_wire(board, positions[m]);
            if (wire) {
                snap->boards[i].wires[k++] = (ts_WireSnapshot) {
                    .pos = positions[m],
                    .layer = wire->layer,
                    .width = wire->width,
                    .value = values[m],
                    .cursor = false,
                };
            }
        }

        // cursor wires
        if (board->cursor.wire.drawing) {
            for (size_t m = 0; m < n_wire_cursor; ++m) {
                snap->boards[i].wires[k++] = (ts_WireSnapshot) {
                    .pos = wire_cursor[m],
                    .layer = board->cursor.selected_wire.layer,
                    .width = board->cursor.selected_wire.width,
                    .value = 0,
                    .cursor = true,
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

#ifndef TRANSISTOR_SANDBOX_H
#define TRANSISTOR_SANDBOX_H

#include <pthread.h>
#include <stdint.h>

#include "sandbox/sandbox.h"

typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_TransistorConfig {
    bool multithreaded;
    bool heavy;
} ts_TransistorConfig;

typedef struct ts_Transistor {
    ts_Sandbox          sandbox;
    ts_TransistorConfig config;

    pthread_t           thread;
    pthread_mutex_t     lock;
    pthread_cond_t      cond;
    bool                thread_running;
    bool                thread_paused;
} ts_Transistor;

typedef unsigned int ts_BoardIdx;

// version
const char* ts_transistor_version(int* major, int* minor, int* patch);

// initialization
ts_Result ts_transistor_init(ts_Transistor* t, ts_TransistorConfig config);
ts_Result ts_transistor_unserialize(ts_Transistor* t, ts_TransistorConfig config, const char* str);
ts_Result ts_transistor_finalize(ts_Transistor* t);

// locks (generally not needed)
ts_Result ts_transistor_lock(ts_Transistor* t);
ts_Result ts_transistor_unlock(ts_Transistor* t);

// boards
ts_BoardIdx ts_transistor_add_board(ts_Transistor* t, int w, int h);

// component db
ts_Result ts_transistor_component_db_add(ts_Transistor* t, ts_ComponentDef const* def);

// execution
ts_Result ts_transistor_run(ts_Transistor* t, size_t run_for_us);

// cursor
ts_Result ts_transistor_cursor_set_pointer(ts_Transistor* t, ts_BoardIdx board_idx, ts_Position pos);
ts_Result ts_transistor_cursor_set_pointer_out_of_bounds(ts_Transistor* t, ts_BoardIdx board_idx);
ts_Result ts_transistor_cursor_click(ts_Transistor* t, ts_BoardIdx board_idx, ts_CursorButton button);
ts_Result ts_transistor_cursor_release(ts_Transistor* t, uint8_t button);
ts_Result ts_transistor_cursor_key_press(ts_Transistor* t, ts_BoardIdx board_idx, char key, uint8_t keymod);
ts_Result ts_transistor_cursor_key_release(ts_Transistor* t, char key);
ts_Result ts_transistor_cursor_select_component_def(ts_Transistor* t, const char* name);

// errors
ts_Result ts_transistor_last_error(ts_Transistor* t, char* err_buf, size_t err_buf_sz);

//
// take snapshot
//

typedef struct ts_PinSnapshot {
    char*      name;
    ts_PinType type;
} ts_PinSnapshot;

typedef struct ts_ComponentSnapshot {
    char*            key;
    ts_Position      pos;
    ts_Direction     direction;
    ts_ComponentType type;
    uint8_t          ic_width;
    uint8_t          n_pins;
    ts_PinSnapshot*  pins;
    void*            extra_data;
    bool             cursor;
} ts_ComponentSnapshot;

typedef struct ts_WireSnapshot {
    ts_Position  pos;
    ts_WireWidth width;
    ts_WireLayer layer;
    uint8_t      value;
    bool         cursor;
} ts_WireSnapshot;

typedef struct ts_BoardSnapshot {
    int                   w;
    int                   h;
    ts_ComponentSnapshot  cursor;
    ts_ComponentSnapshot* components;
    size_t                n_components;
    ts_WireSnapshot*      wires;
    size_t                n_wires;
} ts_BoardSnapshot;

typedef struct ts_TransistorSnapshot {
    ts_BoardSnapshot* boards;
    size_t            n_boards;
} ts_TransistorSnapshot;

ts_Result ts_transistor_take_snapshot(ts_Transistor const* t, ts_TransistorSnapshot* snap);
ts_Result ts_snapshot_finalize(ts_TransistorSnapshot* snap);

#endif //TRANSISTOR_SANDBOX_H

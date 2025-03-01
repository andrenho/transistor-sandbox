#ifndef COMPONENTDEF_H
#define COMPONENTDEF_H

#include <stdbool.h>
#include <stdint.h>

#include "board/board.h"
#include "board/wire.h"

typedef struct ts_Component ts_Component;

typedef enum { TS_SINGLE_TILE, TS_IC_DIP, TS_IC_QUAD } ts_ComponentType;

typedef enum { TS_INPUT, TS_OUTPUT } ts_PinDirection;

typedef struct ts_PinDef {
    char*           name;
    ts_PinDirection direction;
    ts_WireWidth    wire_width;
} ts_PinDef;

typedef struct ts_ComponentDef {
    char*            key;
    ts_ComponentType type;
    bool             can_rotate;
    uint8_t          ic_width;

    uint8_t          n_pins;
    ts_PinDef*       pins;

    size_t           data_size;

    void             (*c_simulate)(ts_Component* component);

    ts_Sandbox*      sandbox;
    int              luaref;

    /*
    void             (*init)(ts_Component* component);

    void             (*on_click)(ts_Component* component);

    int              (*serialize)(ts_Component* component, int vspace, char* buf, size_t buf_sz);
    ts_Result        (*unserialize)(ts_Component* component, lua_State* L, ts_Board* board);
    */

} ts_ComponentDef;

// initialization
typedef void (*SimulateFn)(ts_Component* component);
ts_Result ts_component_def_init_from_lua(ts_ComponentDef* def, SimulateFn sim_fn, ts_Sandbox* sb); // assumes component Lua table in place
ts_Result ts_component_def_finalize(ts_ComponentDef* def);

// positioning
ts_Rect ts_component_def_rect(ts_ComponentDef const* def, ts_Position component_pos, ts_Direction dir);
size_t  ts_component_def_pin_positions(ts_ComponentDef const* def, ts_Position component_pos, ts_Direction dir,
                                       ts_PinPos* pin_pos, size_t max_pin_pos);

#endif //COMPONENTDEF_H

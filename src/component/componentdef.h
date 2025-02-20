#ifndef COMPONENTDEF_H
#define COMPONENTDEF_H

#include <stdbool.h>
#include <stdint.h>

#include "board/wire.h"

typedef struct ts_Component ts_Component;
typedef struct ts_Board ts_Board;

typedef enum { TS_SINGLE_TILE, TS_IC_DIP, TS_IC_QUAD } ts_ComponentType;

typedef enum { TS_INPUT, TS_OUTPUT } ts_PinType;

typedef struct ts_PinDef {
    const char*  name;
    ts_PinType   type;
    ts_WireWidth wire_width;
} ts_PinDef;

typedef struct ts_ComponentDef {
    const char*      name;
    ts_ComponentType type;
    bool             can_rotate;
    uint8_t          ic_width;

    uint8_t          n_pins;
    ts_PinDef*       pins;

    size_t           data_size;

    void             (*init)();

    void             (*on_click)(ts_Component* component);
    void             (*simulate)(ts_Component* component);

    int              (*serialize)(ts_Component* component, int vspace, char* buf, size_t buf_sz);
    ts_Response      (*unserialize)(ts_Component* component, lua_State* L, ts_Board* board);

    void*            extra;
    bool             custom;  // not native

} ts_ComponentDef;

#endif //COMPONENTDEF_H

#ifndef COMPONENT_H
#define COMPONENT_H

#include <lua.h>
#include <stdint.h>

#include "board/position.h"
#include "util/response.h"

typedef struct ts_ComponentDef ts_ComponentDef;
typedef struct ts_Sandbox ts_Sandbox;

typedef struct ts_Component {
    ts_ComponentDef const* def;
    ts_Direction           direction;
    uint8_t*               data;
    uint8_t*               pin_values;
} ts_Component;

// initialization
ts_Response ts_component_init(ts_Component* component, ts_ComponentDef const* def);
ts_Response ts_component_finalize(ts_Component* component);

// serialization
int         ts_component_serialize(ts_Component const* component, int vspace, char* buf, size_t buf_sz);
ts_Response ts_component_unserialize(ts_Component* component, lua_State* L, ts_Sandbox* sb);

#endif //COMPONENT_H

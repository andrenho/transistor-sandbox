#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdint.h>

#include "board/position.h"
#include "util/response.h"

typedef struct ts_ComponentDef ts_ComponentDef;

typedef struct ts_Component {
    ts_ComponentDef const* def;
    ts_Direction           direction;
    uint8_t*               data;
    uint8_t*               pin_values;
} ts_Component;

ts_Response ts_component_init(ts_Component* component, ts_ComponentDef const* def);
ts_Response ts_component_finalize(ts_Component* component);

#endif //COMPONENT_H

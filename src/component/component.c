#include "component.h"

#include <stdlib.h>
#include <string.h>

#include "componentdef.h"

ts_Response ts_component_init(ts_Component* component, ts_ComponentDef const* def)
{
    memset(component, 0, sizeof(ts_Component));
    component->def = def;
    component->direction = TS_N;
    component->data = calloc(1, def->data_size);
    component->pin_values = calloc(def->n_pins, sizeof component->pin_values[0]);
    if (def->init)
        def->init(component);
    return TS_OK;
}

ts_Response ts_component_finalize(ts_Component* component)
{
    free(component->data);
    free(component->pin_values);
    return TS_OK;
}

#include "defaultcomponents.h"

static void vcc_sim(ts_Component* component)
{
    // TODO - vcc.pins[0] = vcc.pins[1] = vcc.pins[2] = vcc.pins[3] = 1;
};

static const ts_ComponentDef vcc = {
    .key        = "vcc",
    .type       = TS_SINGLE_TILE,
    .can_rotate = false,
    .n_pins     = 4,
    .pins       = (ts_PinDef[]) {
        { "O0", TS_OUTPUT, TS_W1 },
        { "O1", TS_OUTPUT, TS_W1 },
        { "O2", TS_OUTPUT, TS_W1 },
        { "O3", TS_OUTPUT, TS_W1 },
    },
    .simulate = vcc_sim,
};

ts_Response ts_add_default_components(ts_ComponentDB* db)
{
    ts_ComponentDef const* defs[] = { &vcc, NULL };

    for (size_t i = 0; defs[i]; ++i) {
        ts_Response r = ts_component_db_add_def(db, defs[i]);
        if (r != TS_OK)
            return r;
    }
    return TS_OK;
}

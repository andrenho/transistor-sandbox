#include "defaultcomponents.h"

//
// vcc
//

static void vcc_sim(ts_Component* vcc)
{
    vcc->pins[0] = vcc->pins[1] = vcc->pins[2] = vcc->pins[3] = 1;
}

static const ts_ComponentDef vcc = {
    .key        = "__vcc",
    .type       = TS_SINGLE_TILE,
    .can_rotate = false,
    .n_pins     = 4,
    .pins       = (ts_PinDef[]) {
        { "O0", TS_OUTPUT, TS_WIRE_1 },
        { "O1", TS_OUTPUT, TS_WIRE_1 },
        { "O2", TS_OUTPUT, TS_WIRE_1 },
        { "O3", TS_OUTPUT, TS_WIRE_1 },
    },
    // .simulate = vcc_sim,
};

//
// button
//

static void button_click(ts_Component* button)
{
    button->data[0] = !button->data[0];
}

static void button_sim(ts_Component* button)
{
    button->pins[0] = button->pins[1] = button->pins[2] = button->pins[3] = button->data[0];
}

static const ts_ComponentDef button = {
    .key = "__button",
    .type = TS_SINGLE_TILE,
    .can_rotate = false,
    .n_pins = 4,
    .pins = (ts_PinDef[]) {
        { "O0", TS_OUTPUT, TS_WIRE_1 },
        { "O1", TS_OUTPUT, TS_WIRE_1 },
        { "O2", TS_OUTPUT, TS_WIRE_1 },
        { "O3", TS_OUTPUT, TS_WIRE_1 },
    },
    .data_size = 1,
    // .on_click = button_click,
    // .simulate = button_sim,
};

//
// led
//

static void led_sim(ts_Component* led)
{
    led->data[0] = led->pins[0] | led->pins[1] | led->pins[2] | led->pins[3];
}

static const ts_ComponentDef led = {
    .key = "__led",
    .type = TS_SINGLE_TILE,
    .can_rotate = false,
    .n_pins = 4,
    .pins = (ts_PinDef[]) {
        { "I0", TS_INPUT, TS_WIRE_1 },
        { "I1", TS_INPUT, TS_WIRE_1 },
        { "I2", TS_INPUT, TS_WIRE_1 },
        { "I3", TS_INPUT, TS_WIRE_1 },
    },
    .data_size = 1,
    // .simulate = led_sim,
};

//
// npn
//

static void npn_sim(ts_Component* npn)
{
    static const uint8_t IN = 1, SW0 = 0, SW1 = 2, OUT = 3;
    npn->pins[OUT] = npn->pins[IN] & (npn->pins[SW0] | npn->pins[SW1]);
}

static const ts_ComponentDef npn = {
    .key = "__npn",
    .type = TS_SINGLE_TILE,
    .can_rotate = true,
    .n_pins = 4,
    .pins = (ts_PinDef[]) {
        { "SW0", TS_INPUT,  TS_WIRE_1 },
        { "IN",  TS_INPUT,  TS_WIRE_1 },
        { "SW1", TS_INPUT,  TS_WIRE_1 },
        { "OUT", TS_OUTPUT, TS_WIRE_1 },
    },
    // .simulate = npn_sim,
};

//
// pnp
//

static void pnp_sim(ts_Component* pnp)
{
    static const uint8_t IN = 1, SW0 = 0, SW1 = 2, OUT = 3;
    pnp->pins[OUT] = pnp->pins[IN] & !(pnp->pins[SW0] | pnp->pins[SW1]);
}

static const ts_ComponentDef pnp = {
    .key = "__pnp",
    .type = TS_SINGLE_TILE,
    .can_rotate = true,
    .n_pins = 4,
    .pins = (ts_PinDef[]) {
        { "SW0", TS_INPUT,  TS_WIRE_1 },
        { "IN",  TS_INPUT,  TS_WIRE_1 },
        { "SW1", TS_INPUT,  TS_WIRE_1 },
        { "OUT", TS_OUTPUT, TS_WIRE_1 },
    },
    // .simulate = pnp_sim,
};

//
// or_2i
//

static void or_2i_sim(ts_Component* c)
{
    static const int IN0 = 0, IN1 = 0, Q_ = 2, Q = 3;
    c->pins[Q] = c->pins[IN0] || c->pins[IN1];
    c->pins[Q_] = !c->pins[Q];
}

static const ts_ComponentDef or_2i = {
    .key = "__or_2i",
    .type = TS_IC_DIP,
    .can_rotate = true,
    .n_pins = 4,
    .ic_width = 1,
    .pins = (ts_PinDef[]) {
        { "IN0", TS_INPUT,  TS_WIRE_1 },
        { "IN1", TS_INPUT,  TS_WIRE_1 },
        { "~Q",  TS_OUTPUT, TS_WIRE_1 },
        { "Q",   TS_OUTPUT, TS_WIRE_1 },
    },
    // .simulate = or_2i_sim,
};

//
// add components
//

ts_Result ts_add_default_components(ts_ComponentDB* db)
{
    ts_ComponentDef const* defs[] = { &vcc, &button, &led, &pnp, &npn, &or_2i, NULL };

    for (size_t i = 0; defs[i]; ++i) {
        ts_Result r = ts_component_db_add_def(db, defs[i]);
        if (r != TS_OK)
            return r;
    }
    return TS_OK;
}

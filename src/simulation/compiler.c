#include "compiler.h"

#include <stb_ds.h>

#include "sandbox/sandbox.h"
#include "simulation.h"
#include "component/component.h"
#include "component/pinpos.h"

#define hmcontains(a, b) (hmgeti(a, b) > 0)

static ts_Position* ts_compiler_find_connected_group(ts_Position start, PositionSet* wires, ts_Position* single_tile_component_pins)
{
    ts_Position* result = NULL;
    PositionSet* to_visit = NULL;  // hashset

    hmput(to_visit, start, true);

    while (hmlen(to_visit) > 0) {
        ts_Position visiting = to_visit[0].key;

        if (hmcontains(wires, visiting)) {

            // add to result list, and remove from visited
            arrpush(result, visiting);
            hmdel(wires, visiting);

            // add neighbours
            // TODO
        }

        hmdel(to_visit, visiting);
    }

    return result;
}

ts_Position** ts_compiler_find_connected_wires(PositionSet* wires, ts_Position* single_tile_component_pins)
{
    if (arrlen(wires) == 0)
        return NULL;

    ts_Position** groups = NULL;
    while (arrlen(wires) > 0)
        arrpush(groups, ts_compiler_find_connected_group(wires[0].key, wires, single_tile_component_pins));

    return groups;
}

ts_Pin* ts_compiler_find_all_pins(ts_Sandbox const* sb)
{
    ts_Pin* pins = NULL;
    for (int i = 0; i < arrlen(sb->boards); ++i) {
        for (int j = 0; j < hmlen(sb->boards[i].components); ++j) {
            ts_Position pos = ts_pos_unhash(sb->boards[i].components[j].key);
            ts_Component* component = &sb->boards[i].components[j].value;
            ts_PinPos pin_pos[component->def->n_pins];
            ts_component_pin_positions(component, pos, pin_pos, component->def->n_pins);

            for (int k = 0; k < component->def->n_pins; ++k) {
                ts_Pin pin = {
                    &sb->boards[i],
                    component,
                    pin_pos[k].pos,
                    pin_pos[k].pin_no
                };
                arrpush(pins, pin);
            }
        }
    }
    return pins;
}

ts_Connection* ts_find_connections(ts_Sandbox const* sb, ts_Pin const* pins)
{
    ts_Connection* connections = NULL;

    // TODO - create set of pin positions

    // TODO - for each pin, create a connection, and remove connecting pins from the list

    return connections;
}

ts_Connection* ts_compiler_compile(ts_Sandbox const* sb)
{
    ts_Pin* pins = ts_compiler_find_all_pins(sb);
    ts_Connection* connections = ts_find_connections(sb, pins);
    arrfree(pins);
    return connections;
}

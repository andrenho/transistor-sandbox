#include "connectedwires.h"

static ts_Position* ts_tile_neighbours(ts_Position visiting, bool has_single_tile_component_pin)
{
    return NULL;
}

static ts_Position* ts_compiler_find_connected_group(ts_Position start, ts_PosSet* wires, ts_Position* single_tile_component_pins)
{
    ts_Position* result = NULL;
    ts_PosSet* to_visit = NULL;  // hashset

    psetput(to_visit, start);

    while (setlen(to_visit) > 0) {
        ts_Position visiting = psetfirst(to_visit);

        if (psetcontains(wires, visiting)) {

            // add to result list, and remove from visited
            arrpush(result, visiting);
            psetdel(wires, visiting);

            // find if we have a single tile component pin here
            bool has_single_tile_component_pin = false; // TODO - check single tile component

            // add neighbours
            ts_Position* neighbours = ts_tile_neighbours(visiting, has_single_tile_component_pin);
            for (int i = 0; i < arrlen(neighbours); ++i)
                psetput(to_visit, neighbours[i]);
            arrfree(neighbours);
        }

        psetdel(to_visit, visiting);
    }

    return result;
}

PositionArray* ts_compiler_find_connected_wires(ts_PosSet* wires, ts_Position* single_tile_component_pins)
{
    if (setlen(wires) == 0)
        return NULL;

    ts_Position** groups = NULL;
    while (setlen(wires) > 0)
        arrpush(groups, ts_compiler_find_connected_group(psetfirst(wires), wires, single_tile_component_pins));

    return groups;
}


#include "doctest.h"

#include <stb_ds.h>
#include <string>


extern "C" {
#include "transistor-sandbox.h"
#include "simulation/compiler.h"
#include "basic/pos_ds.h"

PositionArray* ts_compiler_find_connected_wires(ts_PosSet* wires, ts_Position* single_tile_component_pins);
}

std::string to_string(ts_Position const& p)
{
    return std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.dir);
}

TEST_SUITE("Connected wires")
{
    TEST_CASE("Simple wire")
    {
        ts_PosSet* positions = nullptr;

        ts_Position p0 { 1, 1, TS_E }; psetput(positions, p0);
        ts_Position p1 { 2, 1, TS_W }; psetput(positions, p1);
        ts_Position p2 { 2, 1, TS_E }; psetput(positions, p2);
        ts_Position p3 { 3, 1, TS_W }; psetput(positions, p3);

        PositionArray* groups = ts_compiler_find_connected_wires(positions, nullptr);
        CHECK(arrlen(groups) == 1);
    }
}
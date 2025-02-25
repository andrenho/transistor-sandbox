#include "doctest.h"

#include <stb_ds.h>
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include "sandbox/sandbox.h"
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

TEST_SUITE("Simulation")
{
    struct Fixture {
        ts_Sandbox sb {};

        Fixture() {
            ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 1, 1 }, TS_N);
            ts_board_add_component(&sb.boards[0], "__led", { 3, 1 }, TS_N);
            ts_board_add_wires(&sb.boards[0], { 1, 1 }, { 3, 1 }, TS_HORIZONTAL, { TS_WIRE_1, TS_LAYER_TOP });
        }

        ~Fixture() {
            ts_sandbox_finalize(&sb);
        }

        ts_Component* button() { return ts_board_component(&sb.boards[0], { 1, 1 }); }
        ts_Component* led() { return ts_board_component(&sb.boards[0], { 3, 1 }); }
    };

    TEST_CASE("Single-threaded")
    {
        ts_Position pos[200];
        uint8_t value[200];

        Fixture f;
        ts_simulation_run(&f.sb.simulation, 10000);
        CHECK(f.led()->data[0] == 0);

        ts_component_on_click(f.button());
        ts_simulation_run(&f.sb.simulation, 10000);
        CHECK(f.led()->data[0] != 0);
        size_t sz = ts_board_wires(&f.sb.boards[0], pos, value, 200);
        CHECK(sz == 4);
        CHECK(value[0] != 0);

        ts_component_on_click(f.button());
        ts_simulation_run(&f.sb.simulation, 10000);
        CHECK(f.led()->data[0] == 0);
        sz = ts_board_wires(&f.sb.boards[0], pos, value, 200);
        CHECK(sz == 4);
        CHECK(value[0] == 0);

        size_t steps = ts_simulation_steps(&f.sb.simulation);
        printf("Single-threaded steps: %zu\n", steps);
    }

}

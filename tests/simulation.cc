#include "doctest.h"

#include <stb_ds.h>
#include <unistd.h>

extern "C" {
#include "transistor-sandbox.h"
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

TEST_SUITE("Simulation")
{
    struct Fixture {
        ts_Sandbox sb {};

        Fixture(bool multithreaded) {
            ts_sandbox_init(&sb, { multithreaded, multithreaded });
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

        Fixture f(false);
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
    }

    TEST_CASE("Multithreaded")
    {
        ts_Position pos[200];
        uint8_t value[200];

        Fixture f(true);
        usleep(10000);
        CHECK(f.led()->data[0] == 0);

        ts_component_on_click(f.button());
        usleep(10000);
        CHECK(f.led()->data[0] != 0);
        size_t sz = ts_board_wires(&f.sb.boards[0], pos, value, 200);
        CHECK(sz == 4);
        CHECK(value[0] != 0);

        ts_component_on_click(f.button());
        usleep(10000);
        CHECK(f.led()->data[0] == 0);
        sz = ts_board_wires(&f.sb.boards[0], pos, value, 200);
        CHECK(sz == 4);
        CHECK(value[0] == 0);
    }
}
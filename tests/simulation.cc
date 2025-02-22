#include "doctest.h"

#include <stb_ds.h>

extern "C" {
#include "transistor-sandbox.h"
extern ts_Pin*        ts_compiler_find_all_pins(ts_Sandbox const* sb);
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

TEST_SUITE("Simulation")
{
    TEST_CASE("Basic circuit")
    {
        struct Fixture {
            ts_Sandbox sb {};

            Fixture() {
                ts_sandbox_init(&sb);
                ts_board_add_component(&sb.boards[0], "__button", { 1, 1 }, TS_N);
                ts_board_add_component(&sb.boards[0], "__led", { 3, 1 }, TS_N);
                ts_board_add_wires(&sb.boards[0], { 1, 1 }, { 3, 1 }, TS_HORIZONTAL, { TS_W1, TS_TOP });
            }

            ~Fixture() {
                ts_sandbox_finalize(&sb);
            }
        };

        Fixture f;
        ts_Board* board = &f.sb.boards[0];

        SUBCASE("Board elements")
        {
            CHECK(hmlen(board->components) == 2);
            CHECK(ts_board_component(board, { 1, 1 })->def->key == "__button");
            CHECK(ts_board_component(board, { 3, 1 })->def->key == "__led");

            CHECK(hmlen(board->wires) == 4);
            CHECK(ts_board_wire(board, { 1, 1, TS_E }));
            CHECK(ts_board_wire(board, { 2, 1, TS_W }));
            CHECK(ts_board_wire(board, { 2, 1, TS_E }));
            CHECK(ts_board_wire(board, { 3, 1, TS_W }));
        }

        SUBCASE("Find all pins")
        {
            ts_Pin* pins = ts_compiler_find_all_pins(&f.sb);

            auto get = [&](ts_Position pos, uint8_t pin_no) -> ts_Component* {
                for (int i = 0; i < arrlen(pins); ++i)
                    if (ts_pos_equals(pins[i].pos, pos) && pins[i].pin_no == pin_no)
                        return pins[i].component;
                return nullptr;
            };

            CHECK(arrlen(pins) == 8);
            CHECK(get({ 1, 1, TS_N }, 0)->def->key == "__button");
            CHECK(get({ 1, 1, TS_W }, 1)->def->key == "__button");
            CHECK(get({ 1, 1, TS_S }, 2)->def->key == "__button");
            CHECK(get({ 1, 1, TS_E }, 3)->def->key == "__button");
            CHECK(get({ 3, 1, TS_N }, 0)->def->key == "__led");
            CHECK(get({ 3, 1, TS_W }, 1)->def->key == "__led");
            CHECK(get({ 3, 1, TS_S }, 2)->def->key == "__led");
            CHECK(get({ 3, 1, TS_E }, 3)->def->key == "__led");

            arrfree(pins);
        }
    }
}

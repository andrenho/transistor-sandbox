#include "doctest.h"
#include "stb_ds.h"


extern "C" {
#include "transistor-sandbox.h"
}

TEST_SUITE("Compilation")
{
    TEST_CASE("Basic circuit")
    {
        struct Fixture {
            ts_Sandbox sb;

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
        }
    }
}

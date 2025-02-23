#include "doctest.h"

#include <stb_ds.h>

extern "C" {
#include "transistor-sandbox.h"
#include "basic/pos_ds.h"

extern ts_PosSet* ts_board_occupied_tiles(ts_Board const* board);
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

TEST_SUITE("Placement")
{
    TEST_CASE("Wire placement")
    {
        SUBCASE("Don't place outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_wire(&sb.boards[0], { 10, 1, TS_E }, { TS_W1, TS_TOP });
            CHECK(ts_board_wire(&sb.boards[0], { 10, 1, TS_E }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place on top of ICs")
        {
            // TODO
        }

        SUBCASE("Remove wire")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_wires(&sb.boards[0], { 1, 1 }, { 3, 1 }, TS_HORIZONTAL, { TS_W1, TS_TOP });
            ts_board_clear_tile(&sb.boards[0], { 2, 1 });
            CHECK(ts_board_wire(&sb.boards[0], { 1, 1, TS_E }) != NULL);
            CHECK(ts_board_wire(&sb.boards[0], { 2, 1, TS_W }) == NULL);
            CHECK(ts_board_wire(&sb.boards[0], { 2, 1, TS_E }) == NULL);
            CHECK(ts_board_wire(&sb.boards[0], { 3, 1, TS_W }) != NULL);
            ts_sandbox_finalize(&sb);
        }
    }

    TEST_CASE("Single-tile placement")
    {
        SUBCASE("Don't place outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 10, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 10, 1 }) == NULL);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 0, 0 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 0, 0 }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place single-tile component on top of another single-tile component")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 1, 1 }, TS_N);
            ts_board_add_component(&sb.boards[0], "__led", { 1, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 1, 1 })->def->key == "__button");
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place single-tile component on top of IC component")
        {
            // TODO
        }

        SUBCASE("Remove component")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 1, 1 }, TS_N);
            ts_board_clear_tile(&sb.boards[0], { 1, 1 });
            CHECK(ts_board_component(&sb.boards[0], { 1, 1 }) == NULL);
            ts_sandbox_finalize(&sb);
        }
    }

    TEST_CASE("IC placement")
    {
        SUBCASE("Occupied tiles")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);

            ts_board_add_component(&sb.boards[0], "__button", { 0, 0 }, TS_N);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 2, 2 }, TS_N);

            ts_PosSet* tiles = ts_board_occupied_tiles(&sb.boards[0]);
            CHECK(psetlen(tiles) == 9);
            psetcontains_f(tiles, { 0, 0 });
            psetcontains_f(tiles, { 1, 0 });
            psetcontains_f(tiles, { 1, 1 });
            psetcontains_f(tiles, { 1, 2 });
            psetcontains_f(tiles, { 1, 3 });
            psetcontains_f(tiles, { 2, 0 });
            psetcontains_f(tiles, { 2, 1 });
            psetcontains_f(tiles, { 2, 2 });
            psetcontains_f(tiles, { 2, 3 });
            psetfree(tiles);

            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Query IC in a different tile")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 2, 2 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 1, 1 }) != NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 11, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 11, 1 }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place any part of IC outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 1, 9 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 1, 9 }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place IC over single-tile component")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 2, 2 }, TS_N);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 1, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 1, 1 }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place IC over another IC")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 2, 2 }, TS_N);
            ts_board_add_component(&sb.boards[0], "__or_2i", { 1, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 1, 1 }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Overwrite wire on placement")
        {
            // TODO
        }

        SUBCASE("Remove IC")
        {
            // TODO
        }
    }
}
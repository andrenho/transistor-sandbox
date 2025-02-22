#include "doctest.h"

extern "C" {
#include "transistor-sandbox.h"
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

TEST_SUITE("Placement")
{
    TEST_CASE("Wire placement")
    {
        SUBCASE("Don't place outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_wire(&sb.boards[0], { 11, 1, TS_E }, { TS_W1, TS_TOP });
            CHECK(ts_board_wire(&sb.boards[0], { 11, 1, TS_E }) == NULL);
            ts_sandbox_finalize(&sb);
        }

        SUBCASE("Don't place on top of ICs")
        {
            // TODO
        }

        SUBCASE("Remove wire")
        {
            // TODO
        }
    }

    TEST_CASE("Single-tile placement")
    {
        SUBCASE("Don't place outside of circuit bounds")
        {
            ts_Sandbox sb; ts_sandbox_init(&sb);
            ts_board_add_component(&sb.boards[0], "__button", { 11, 1 }, TS_N);
            CHECK(ts_board_component(&sb.boards[0], { 11, 1 }) == NULL);
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
            // TODO
        }
    }

    TEST_CASE("IC placement")
    {
        SUBCASE("Don't place outside of circuit bounds")
        {
        }

        SUBCASE("Don't place IC over single-tile component")
        {
        }

        SUBCASE("Don't place IC over another IC")
        {
        }

        SUBCASE("Overwrite wire on placement")
        {
        }

        SUBCASE("Remove IC")
        {
        }
    }
}
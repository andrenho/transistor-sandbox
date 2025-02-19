#include "doctest.h"

extern "C" {
#include "transistor-sandbox.h"
}

TEST_SUITE("Position & Wires")
{
    TEST_CASE("Position hashing")
    {
        ts_Position pos = { 1492, 328, TS_W };
        ts_Position pos2 = ts_pos_unhash(ts_pos_hash(pos));
        CHECK(pos.x == pos2.x);
        CHECK(pos.y == pos2.y);
        CHECK(pos.dir == pos2.dir);
    }

    TEST_CASE("Wire placement")
    {
        ts_Sandbox sb;
        ts_sandbox_init(&sb);
        ts_board_add_wire(&sb.boards[0], { 1, 1, TS_S }, { TS_W1, TS_TOP });

        ts_Response response = ts_board_add_wire(&sb.boards[0], { 1, 11, TS_S }, { TS_W1, TS_TOP });
        CHECK(response == TS_CANNOT_PLACE);

        CHECK(ts_board_wire(&sb.boards[0], { 1, 1, TS_S })->layer == TS_TOP);
        CHECK(ts_board_wire(&sb.boards[0], { 1, 2, TS_S }) == NULL);
    }

    TEST_CASE("A to B: horizontal")
    {
        ts_Position pos[20];
        size_t sz = ts_pos_a_to_b({ 1, 1, TS_CENTER }, { 2, 3, TS_CENTER }, TS_HORIZONTAL, pos, 20);
        CHECK(sz == 6);
        CHECK(ts_pos_equals(pos[0], { 1, 1, TS_E }));
        CHECK(ts_pos_equals(pos[1], { 2, 1, TS_W }));
        CHECK(ts_pos_equals(pos[2], { 2, 1, TS_S }));
        CHECK(ts_pos_equals(pos[3], { 2, 2, TS_N }));
        CHECK(ts_pos_equals(pos[4], { 2, 2, TS_S }));
        CHECK(ts_pos_equals(pos[5], { 2, 3, TS_N }));
    }

    TEST_CASE("A to B: vertical")
    {
        ts_Position pos[20];
        size_t sz = ts_pos_a_to_b({ 1, 1, TS_CENTER }, { 2, 3, TS_CENTER }, TS_VERTICAL, pos, 20);
        CHECK(sz == 6);
        CHECK(ts_pos_equals(pos[0], { 1, 1, TS_S }));
        CHECK(ts_pos_equals(pos[1], { 1, 2, TS_N }));
        CHECK(ts_pos_equals(pos[2], { 1, 2, TS_S }));
        CHECK(ts_pos_equals(pos[3], { 1, 3, TS_N }));
        CHECK(ts_pos_equals(pos[4], { 2, 3, TS_E }));
        CHECK(ts_pos_equals(pos[5], { 2, 3, TS_W }));
    }
}
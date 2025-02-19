#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
#include "sandbox/sandbox.h"
#include "board/board.h"
#include "board/position.h"
}

TEST_SUITE("Sandbox") {

    TEST_CASE("Position hashing")
    {
        ts_Position pos = { 1492, 328, TS_W };
        ts_Position pos2 = ts_pos_unhash(ts_pos_hash(&pos));
        CHECK(pos.x == pos2.x);
        CHECK(pos.y == pos2.y);
        CHECK(pos.dir == pos2.dir);
    }

    TEST_CASE("Serialization / unserialization") {
        ts_Sandbox sb;
        ts_sandbox_init(&sb);

        char serialized[4096] = "return ";
        ts_sandbox_serialize(&sb, 0, &serialized[7], sizeof serialized - 7);
        printf("%s\n", serialized);

        ts_Sandbox sb2;
        ts_Response response = ts_sandbox_unserialize_from_string(&sb2, serialized);
        if (response != TS_OK)
            printf("%s\n", ts_last_error(&sb2, nullptr));

        CHECK(response == TS_OK);
        CHECK(sb.boards[0].w == sb2.boards[0].w);
        CHECK(sb.boards[0].h == sb2.boards[0].h);
    }

}

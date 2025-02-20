#include "doctest.h"

#include <cstdio>

#include "stb_ds.h"


extern "C" {
#include "transistor-sandbox.h"
}

TEST_SUITE("Serialization") {

    TEST_CASE("Serialization / unserialization")
    {
        ts_Sandbox sb;
        ts_sandbox_init(&sb);

        ts_board_add_wire(&sb.boards[0], { 1, 1, TS_S }, { TS_W1, TS_TOP });
        ts_board_add_component(&sb.boards[0], "vcc", { 2, 2, TS_CENTER }, TS_E);

        char serialized[4096] = "return ";
        ts_sandbox_serialize(&sb, 0, &serialized[7], sizeof serialized - 7);
        printf("%s\n", serialized);

        ts_Sandbox sb2;
        ts_Response response = ts_sandbox_unserialize_from_string(&sb2, serialized);
        if (response != TS_OK)
            printf("%s\n", ts_last_error(&sb2, nullptr));

        CHECK(shlen(sb.component_db.items) == shlen(sb2.component_db.items));

        CHECK(response == TS_OK);
        CHECK(sb.boards[0].w == sb2.boards[0].w);
        CHECK(sb.boards[0].h == sb2.boards[0].h);

        CHECK(ts_board_wire(&sb2.boards[0], { 1, 1, TS_S })->layer == TS_TOP);
        CHECK(ts_board_wire(&sb2.boards[0], { 1, 2, TS_S }) == NULL);

        ts_Component* component = ts_board_component(&sb2.boards[0], { 2, 2, TS_CENTER });
        CHECK(component != nullptr);
        CHECK(component->def->key == "vcc");
        CHECK(component->direction == TS_E);

        ts_sandbox_finalize(&sb2);
        ts_sandbox_finalize(&sb);
    }

}

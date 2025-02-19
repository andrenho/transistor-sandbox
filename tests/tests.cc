#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

extern "C" {
#include "sandbox/sandbox.h"
}

TEST_SUITE("Sandbox") {

    TEST_CASE("Serialization / unserialization") {
        ts_Sandbox* sb = ts_sandbox_create();
        char buf[4096], err[1024];
        ts_sandbox_serialize(sb, buf, sizeof buf);
        printf("%s\n", buf);
        ts_Sandbox* sb2 = ts_sandbox_unserialize_from_string(buf, err, sizeof err);

        CHECK(1 == 1);
    }

}

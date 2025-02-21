#include "doctest.h"

#include <cstdlib>
#include <string>

extern "C" {
#include "transistor-sandbox.h"
}

struct Fixture {

    Fixture(ts_ComponentType type, uint8_t n_pins)
    {
        def.type = type;
        def.n_pins = n_pins;

        ts_component_init(&component, &def);
    }

    ~Fixture()
    {
        ts_component_finalize(&component);
    }

    ts_Component component {};

    ts_ComponentDef def = {
        .can_rotate = true,
        .pins = (ts_PinDef[]) {
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
                { "", TS_INPUT, TS_W1 },
            },
    };

};

TEST_SUITE("Pin positions")
{
    ts_PinPos pins[20];

    TEST_CASE("Single-tile 1 pin")
    {
        Fixture f(TS_SINGLE_TILE, 1);

        SUBCASE("Direction N")
        {
            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);

            CHECK(sz == 1);
            CHECK(pins[0].pin_no == 0);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 1, TS_N }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);
            CHECK(pins[0].pos.dir == TS_E);
        }
    }

    TEST_CASE("Single-tile 2 pin")
    {
        Fixture f(TS_SINGLE_TILE, 2);

        SUBCASE("Direction N")
        {
            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);

            CHECK(sz == 2);
            CHECK(pins[0].pin_no == 0);
            CHECK(pins[1].pin_no == 1);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 1, TS_N }));
            CHECK(ts_pos_equals(pins[1].pos, { 1, 1, TS_S }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);
            CHECK(sz == 2);
            CHECK(pins[0].pos.dir == TS_E);
            CHECK(pins[1].pos.dir == TS_W);
        }
    }

    TEST_CASE("Single-tile 4 pin")
    {
        Fixture f(TS_SINGLE_TILE, 4);

        SUBCASE("Direction N")
        {
            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);

            CHECK(sz == 4);
            CHECK(pins[0].pin_no == 0);
            CHECK(pins[1].pin_no == 1);
            CHECK(pins[2].pin_no == 2);
            CHECK(pins[3].pin_no == 3);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 1, TS_N }));
            CHECK(ts_pos_equals(pins[1].pos, { 1, 1, TS_W }));
            CHECK(ts_pos_equals(pins[2].pos, { 1, 1, TS_S }));
            CHECK(ts_pos_equals(pins[3].pos, { 1, 1, TS_E }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);
            CHECK(sz == 4);
            CHECK(pins[0].pos.dir == TS_E);
            CHECK(pins[1].pos.dir == TS_N);
            CHECK(pins[2].pos.dir == TS_W);
            CHECK(pins[3].pos.dir == TS_S);
        }
    }
}
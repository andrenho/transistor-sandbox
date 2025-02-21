#include "doctest.h"

#include <cstdlib>
#include <string>

extern "C" {
#include "transistor-sandbox.h"
}

static ts_PinPos pins[20];

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

    TEST_CASE("DIP component - 6 pins")
    {
        Fixture f(TS_IC_DIP, 6);

        SUBCASE("Direction N")
        {
            ts_Rect rect = ts_component_rect(&f.component, (ts_Position) { 1, 1, TS_N });
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 2, 4, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 0, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 2, 1, TS_CENTER }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            ts_Rect rect = ts_component_rect(&f.component, (ts_Position) { 1, 1, TS_N });
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 4, 2, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, (ts_Position) { 1, 1, TS_N }, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 3, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 1, 0, TS_CENTER }));
        }
    }
}
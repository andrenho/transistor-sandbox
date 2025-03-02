#include "doctest.h"

#include <cstdlib>
#include <string.h>
#include <string>

extern "C" {
#include "sandbox/sandbox.h"
}

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

static ts_PinPos pins[20];

struct PPFixture {

    PPFixture(ts_ComponentType type, uint8_t n_pins, int width=1)
    {
        ts_sandbox_init(&sandbox);
        def.type = type;
        def.n_pins = n_pins;
        def.ic_width = width;
        def.data_size = 0;
        def.sandbox = &sandbox;

        ts_component_init(&component, &def, TS_N);
    }

    ~PPFixture()
    {
        ts_component_finalize(&component);
        ts_sandbox_finalize(&sandbox);
    }

    ts_Sandbox sandbox;
    ts_Component component {};

    ts_ComponentDef def = {
        .key = strdup(""),
        .can_rotate = true,
        .ic_width = 1,
        .pins = (ts_PinDef[]) {
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
            { strdup(""), TS_INPUT, TS_WIRE_1 },
        },
    };

};


TEST_SUITE("Pin positions")
{
    TEST_CASE("Single-tile 1 pin")
    {
        PPFixture f(TS_SINGLE_TILE, 1);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 1);
            CHECK(pins[0].pin_no == 0);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 1, TS_N }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;
            f.component.position = { 1, 1, TS_N };
            ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(pins[0].pos.dir == TS_E);
        }
    }

    TEST_CASE("Single-tile 2 pin")
    {
        PPFixture f(TS_SINGLE_TILE, 2);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 2);
            CHECK(pins[0].pin_no == 0);
            CHECK(pins[1].pin_no == 1);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 1, TS_N }));
            CHECK(ts_pos_equals(pins[1].pos, { 1, 1, TS_S }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;
            f.component.position = { 1, 1, TS_N };
            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 2);
            CHECK(pins[0].pos.dir == TS_E);
            CHECK(pins[1].pos.dir == TS_W);
        }
    }

    TEST_CASE("Single-tile 4 pin")
    {
        PPFixture f(TS_SINGLE_TILE, 4);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

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
            f.component.position = { 1, 1, TS_N };
            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 4);
            CHECK(pins[0].pos.dir == TS_E);
            CHECK(pins[1].pos.dir == TS_N);
            CHECK(pins[2].pos.dir == TS_W);
            CHECK(pins[3].pos.dir == TS_S);
        }
    }

    TEST_CASE("DIP component - 6 pins")
    {
        PPFixture f(TS_IC_DIP, 6);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 2, 4, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

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

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 4, 2, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

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

        SUBCASE("Direction S")
        {
            f.component.direction = TS_S;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 2, 4, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 2, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 0, 3, TS_CENTER }));
        }

        SUBCASE("Direction W")
        {
            f.component.direction = TS_W;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 4, 2, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 3, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 1, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 1, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 3, 2, TS_CENTER }));
        }
    }

    TEST_CASE("DIP component - 6 pins, width 2")
    {
        PPFixture f(TS_IC_DIP, 6, 2);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 4, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 0, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 3, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 3, 1, TS_CENTER }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 4, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 3, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 3, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 1, 0, TS_CENTER }));
        }

        SUBCASE("Direction S")
        {
            f.component.direction = TS_S;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 4, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 3, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 3, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 0, 3, TS_CENTER }));
        }

        SUBCASE("Direction W")
        {
            f.component.direction = TS_W;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 4, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 6);
            for (int i = 0; i < 6; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 3, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 1, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 3, 3, TS_CENTER }));
        }
    }

    TEST_CASE("QUAD component - 8 pins")
    {
        PPFixture f(TS_IC_QUAD, 8);

        SUBCASE("Direction N")
        {
            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 8);
            for (int i = 0; i < 8; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 3, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[6].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[7].pos, { 1, 0, TS_CENTER }));
        }

        SUBCASE("Direction E")
        {
            f.component.direction = TS_E;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 8);
            for (int i = 0; i < 8; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 1, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[6].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[7].pos, { 3, 1, TS_CENTER }));
        }

        SUBCASE("Direction S")
        {
            f.component.direction = TS_S;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 8);
            for (int i = 0; i < 8; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 3, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 1, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 0, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[6].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[7].pos, { 2, 3, TS_CENTER }));
        }

        SUBCASE("Direction W")
        {
            f.component.direction = TS_W;

            f.component.position = { 1, 1, TS_N };
            ts_Rect rect = ts_component_rect(&f.component);
            CHECK(ts_pos_equals(rect.top_left, { 0, 0, TS_CENTER }));
            CHECK(ts_pos_equals(rect.bottom_right, { 3, 3, TS_CENTER }));

            size_t sz = ts_component_pin_positions(&f.component, pins, sizeof pins);

            CHECK(sz == 8);
            for (int i = 0; i < 8; ++ i)
                CHECK(pins[i].pin_no == i);
            CHECK(ts_pos_equals(pins[0].pos, { 1, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[1].pos, { 2, 3, TS_CENTER }));
            CHECK(ts_pos_equals(pins[2].pos, { 3, 2, TS_CENTER }));
            CHECK(ts_pos_equals(pins[3].pos, { 3, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[4].pos, { 2, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[5].pos, { 1, 0, TS_CENTER }));
            CHECK(ts_pos_equals(pins[6].pos, { 0, 1, TS_CENTER }));
            CHECK(ts_pos_equals(pins[7].pos, { 0, 2, TS_CENTER }));
        }
    }
}

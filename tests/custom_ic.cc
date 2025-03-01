#include <lua.hpp>

#include "doctest.h"

#include <string>

extern "C" {
#include "sandbox/sandbox.h"
#include "component/componentdef.h"
}

std::string button = R"({
    key = "__button",
    type = "single_tile",

    can_rotate = false,
    data_size = 1,

    pins = {
        { name = "O1", direction = "output" },
        { name = "O2", direction = "output" },
        { name = "O3", direction = "output" },
        { name = "O4", direction = "output" },
    },

    on_click = function(button)
        button.data[1] = bit.bnot(button.data[1])
    end,

    simulate = function(button)
        --print(#button.pin)
        --print(button.pin[1])
        --print(button.data[1])
        --for i=1,#button.pin do button.pin[i] = button.data[1] end
        button.pin[1] = button.data[1]
        print(button.pin[1])
    end,

    -- rendering

    init = function(button, renderer)
        puts('Button initialized.')
    end,

    render = function(button, renderer, x, y) end,
})";

TEST_SUITE("Load IC from Lua")
{

    TEST_CASE("Custom single-tile data")
    {
        ts_Sandbox sb;
        ts_sandbox_init(&sb);

        luaL_loadstring(sb.L, ("return " + button).c_str());
        lua_call(sb.L, 0, 1);

        ts_ComponentDef def;
        ts_Result r = ts_component_def_init_from_lua(&def, nullptr, &sb);
        if (r != TS_OK)
            fprintf(stderr, "error: %s\n", ts_last_error(&sb, NULL));

        CHECK(r == TS_OK);
        CHECK(std::string(def.key) == "__button");
        CHECK(def.type == TS_SINGLE_TILE);
        CHECK(def.can_rotate == false);
        CHECK(def.ic_width == 1);
        CHECK(def.data_size == 1);
        CHECK(def.n_pins == 4);
        CHECK(std::string(def.pins[1].name) == "O2");

        ts_component_def_finalize(&def);
    }

    TEST_CASE("Custom IC functions (Lua)")
    {
        ts_Sandbox sb;
        ts_sandbox_init(&sb);

        luaL_loadstring(sb.L, ("return " + button).c_str());
        lua_call(sb.L, 0, 1);

        ts_ComponentDef def;
        ts_component_def_init_from_lua(&def, nullptr, &sb);
        ts_Component component;
        ts_component_init(&component, &def, TS_N);

        CHECK(component.data[0] == 0);
        if (ts_component_on_click(&component) != TS_OK)
            fprintf(stderr, "error: %s\n", ts_last_error(&sb, nullptr));
        CHECK(component.data[0] != 0);

        CHECK(component.pins[0] == 0);
        if (ts_component_simulate(&component) != TS_OK)
            fprintf(stderr, "error: %s\n", ts_last_error(&sb, nullptr));
        CHECK(component.pins[0] != 0);

        ts_component_def_finalize(&def);
    }

    TEST_CASE("Custom simulation (C)")
    {
    }

    TEST_CASE("Multiple ICs")
    {
    }
}
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
        { name = "O1", type = "output", wire_width = 1 },
        { name = "O2", type = "output", wire_width = 1 },
        { name = "O3", type = "output", wire_width = 1 },
        { name = "O4", type = "output", wire_width = 1 },
    },

    on_click = function(button)
        button.data[1] = not button.data[1]
    end,

    simulate = function(button)
        for i in 1,4 do button.pin[i] = button.data[1] end
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
        lua_State* L = luaL_newstate();
        ts_Sandbox sb;
        ts_sandbox_init(&sb);

        luaL_loadstring(L, ("return " + button).c_str());
        lua_call(L, 0, 1);

        ts_ComponentDef def;
        ts_Result r = ts_component_def_load(&def, L, nullptr, &sb);
        if (r != TS_OK)
            fprintf(stderr, "%s\n", ts_last_error(&sb, NULL));

        CHECK(r == TS_OK);
        CHECK(std::string(def.key) == "__button");
        CHECK(def.type == TS_SINGLE_TILE);
        CHECK(def.can_rotate == false);
        CHECK(def.ic_width == 1);
        CHECK(def.data_size == 1);
        CHECK(def.n_pins == 4);
        CHECK(std::string(def.pins[1].name) == "02");

        ts_component_def_finalize(&def);
        lua_close(L);
    }

    TEST_CASE("Custom IC functions (Lua)")
    {
    }

    TEST_CASE("Custom simulation (C)")
    {
    }

    TEST_CASE("Multiple ICs")
    {
    }
}
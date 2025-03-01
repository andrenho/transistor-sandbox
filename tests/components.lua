local bit = require("bit")
local bor = bit.bor

return {
   
   --
   -- BUTTON
   --
   
   {
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
   },
   
   --
   -- LED
   --
   
   {
      key = "__led",
      type = "single_tile",
      
      can_rotate = false,
      data_size = 1,
      
      pins = {
         { name = "O1", type = "output", wire_width = 1 },
         { name = "O2", type = "output", wire_width = 1 },
         { name = "O3", type = "output", wire_width = 1 },
         { name = "O4", type = "output", wire_width = 1 },
      },
      
      simulate = function(led)
         led.data[1] = bor(led.pins[1], led.pins[2], led.pins[3], led.pins[4])
      end,
      
      -- rendering
      
      init = function(button, renderer)
         puts('Button initialized.')
      end,
      
      render = function(button, renderer, x, y) end,
   },
   
}
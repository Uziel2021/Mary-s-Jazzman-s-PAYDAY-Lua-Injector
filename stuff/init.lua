local dofile = dofile

dofile('__require2.lua') --Loading improved require function
--[[
--__first_require_clbk deprecated!

--Callbacks, these executed before require script being executed
__require_pre[required_script] = callback_function

--Callbacks, these executed after required script being executed
__require_after[required_script] = callback_function2

--Callbacks, these will override whole require
__require_override[required_script] = callback_function3

Example:
__require_after['lib/entry'] = function()
	dofile("somescript1.lua")
	dofile("somescript2.lua")
	--Futher code maybe
end
--When lib/entry will be fully required, 2 scripts files "somescript1.lua" & "somescript2.lua" will be executed.
]]

--Anything else, that needs to be executed on newstate goes here. Keep in mind, that only lua libs are opened at this stage, none of game internal classes, objects, methods are initialized yet.
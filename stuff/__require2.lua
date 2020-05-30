--Purpose: improved require function. Now synced more with original's lua one.
--Author: ThisJazzman

local orig__require = orig__require
if ( not orig__require ) then
	local _G = _G
	orig__require = require
	_G.orig__require = orig__require
	local P = package
	local Loaded = P.loaded
	local Loaders = P.loaders
	do
		--Just tries to load filename that was given
		local loadfile = loadfile
		table.insert(Loaders, 1, function( v )
				local l,err = loadfile(v)
				if ( l ) then
					return l
				else
					return err
				end
			end)
	end
	local type = type
	local pcall = pcall
	local assert = assert
	local unpack = unpack
	local pairs = pairs
	local str_lower = string.lower
	local before_require = {}
	local after_require = {}
	local override_require = {}
	P.path = "?.lua;?.luac;"
	P.cpath = "libs/?.dll;"

	--Register tables
	_G.__require_pre = before_require
	_G.__require_after = after_require
	_G.__require_override = override_require
	
--	local function pause(v)
--		local lock = os.clock()
--		while(os.clock() - lock < v)do end
--	end

--	local function inspect(v)
--		for k,v in pairs(v) do
--			logme("["..tostring(k).."] => "..tostring(v).."\n")
--		end
--	end
	
	function require( mod )
		local lmod = str_lower(mod)
		local overrided_func = override_require[lmod]
		if ( overrided_func ) then
			return overrided_func()
		end
		local value = Loaded[mod]
		if ( value ) then
			return unpack(value)
		else
			--local accumulate = ''
			local final
			local final_type
			--Iterate through loaders
			for _,loader in pairs( Loaders ) do
				final = loader(mod)
				final_type = type(final)
				if ( final_type == 'function' ) then
					break
				--elseif ( final_type == 'string' ) then
				--	accumulate = accumulate..final
				end
			end
			local before_func = before_require[lmod]
			if ( before_func ) then
				before_func()
			end
			local after_func = after_require[lmod]
			local res
			if ( final_type == 'function' ) then
				--Call loader and catch all results
				res = { final() }
				if ( after_func ) then
					after_func()
				end
			else
				--logme(accumulate)
				--pause(0.1)
				res = { orig__require( mod ) }
				if ( after_func ) then
					after_func()
				end
			end
			Loaded[mod] = res
			return unpack(res)
		end
	end
end
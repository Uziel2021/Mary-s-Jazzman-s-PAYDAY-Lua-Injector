/*
	Underground light Lua hook for PAYDAY 2 & PAYDAY: The Heist
	Copyright(C) 2014-2015 Developed by Mary <75e45b09@anon.leemail.me> & Jazzman <jazzman@thepiratebay.army>

	This program is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/gpl.txt>.
*/

//It survived very dirty and quick MinGW port. No C++ functions should be ever encountered, can be easy compiled with MinGW gcc.

#include "main.h" //Main header
#include "tools.h" //Different helper functions
#include "Console.h"
#include <process.h> //For lua_newthread




#ifdef detour_hooks
#include <detours.h> //Library to hook into different functions
#pragma comment(lib, "detours.lib")
#endif

#include "lua_init.h" //Lua methods
#include "lightfx.h"

#if CanTestPerformance
#include <time.h>
#endif

#ifdef TestHookFunc
struct hHook st_luaL_newstate;
#endif

#ifndef LuaEntryFile
char LuaEntryFile[MAX_PATH];
#endif

#pragma region Lua C extensions

//Lua input: [ ... : any ]
int reLogMe(lua_State * L)
{
	int count = lua_gettop(L);
	if (count)
		for (int pos = 0; pos < count; pos++)
			if (lua_type(L, pos + 1) == LUA_TSTRING)
			{
				const char * str = lua_tolstring(L, pos + 1, 0);
				LOG("%s", str);
			}
	//OUTS << "\n";
	return 0;
}

//Lua input: [ToProtectFunc : function], [ ... : any ]
//Lua output: (Success: _Boolean), [ error_message : string; Returned, if Success is 0 ] or [ ... : any, if Success is 1 ]
int reluaB_pcall(lua_State * L) {
	int status;
	if (lua_gettop(L) == 0)
	{
		lua_pushboolean(L, 0);
		lua_pushstring(L, "Arguments expected");
		return 2;
	}
	status = lua_pcall(L, lua_gettop(L) - 1, LUA_MULTRET, 0);
	lua_pushboolean(L, (status == 0));
	lua_insert(L, 1);
	return lua_gettop(L);  /* return status + all results */
}

//Lua input: (ErrorHandler : function), [ToProtectFunc : function], [ ... : any ]
//Lua output: (Success : _Boolean), [ error_message : string; Returned, if Success is 0 ] or [ ... : any; if Success is 1 ]
int reluaB_xpcall(lua_State * L) { //
	int top = lua_gettop(L);
	if (!top)
	{
		lua_pushboolean(L, 0);
		lua_pushstring(L, "Arguments expected");
		return 2;
	}
	if (lua_isnoneornil(L, 1))
	{
		lua_pushboolean(L, 0);
		lua_pushstring(L, "Handler function required (1st argument)");
		return 2;
	}
	int status;

	status = lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 1);
	lua_pushboolean(L, (status == 0));
	lua_replace(L, 1);
	return lua_gettop(L);  /* return status + all results */
}

//Lua input: (filename : string)
//Lua output: (... : any)
int reDofile(lua_State * L)
{
	int n = lua_gettop(L);
	char copy_name[MAX_PATH];

	if (n && lua_type(L, 1) == LUA_TSTRING)
	{
		lua_settop(L, 1); //Clear unneeded elements from stack
		const char *fname = lua_tolstring(L, 1, 0);
		int error_loading = luaL_loadfile(L, fname);
		const char * errstr = NULL;
		//Probably filename without extension, try to add ".lua" at the end
		if (error_loading == LUA_ERRFILE)
		{
			errstr = lua_tolstring(L, n + 1, 0); //Catch error message from 1st fail for correct debug output
			lua_settop(L, 1);
			strcpy(copy_name, fname);
			strcat(copy_name, ".lua");
			error_loading = luaL_loadfile(L, copy_name);
			//Try to add ".luac"
			if (error_loading == LUA_ERRFILE)
			{
				lua_settop(L, 1);
				strcpy(copy_name, fname);
				strcat(copy_name, ".luac");
				error_loading = luaL_loadfile(L, copy_name);
			}
		}
		//Nothing worked, return error
		if (error_loading)
		{
			if (error_loading != LUA_ERRFILE)
				errstr = lua_tolstring(L, n + 1, 0); //Fail isn't related to LUA_ERRFILE, catch what really caused it

			if (errstr)
				LOG("%s\n", errstr);
			return 0;
		}
		if (lua_pcall(L, 0, LUA_MULTRET, 0))
		{
			errstr = lua_tolstring(L, n + 1, 0);
			if (errstr)
				LOG("%s\n", errstr);
			return 0;
		}
		return lua_gettop(L) - n;
	}
	return 0;
}

//Lua input: (string)filename
//Lua output: (function)chunk/nil[, (string)error_message]
int luaB_loadfile(lua_State * L)
{
	int top = lua_gettop(L);
	if (top && lua_type(L, 1) == LUA_TSTRING)
	{
		const char * fname = lua_tolstring(L, 1, 0);
		if (luaL_loadfile(L, fname))
		{
			lua_pushnil(L);
			lua_insert(L, top+1); //Insert into top
			return 2;
		}
		else
			return 1;
	}
	lua_pushnil(L);
	lua_pushstring(L, "Invalid/no arguments received");
	return 2;
}

static void threaded_lua_call(void * A)
{
	lua_call((lua_State*)A, 0, 0);
}

//Lua input: Lua function
//Lua output: thread object
int LuaCreateThread(lua_State * L)
//About setglobals, don't know any other way on how to share function with other state yet.
//TO DO: Add methods to thread object, these give you brief info about its state.
{
	if (lua_gettop(L) && lua_type(L, 1) == LUA_TFUNCTION)
	{
		lua_setglobal(L, "zzzzzzzzzzzAP");
		lua_State * OL = lua_newthread(L);
		lua_getglobal(OL, "zzzzzzzzzzzAP");
		_beginthread(threaded_lua_call, 0, OL);
		return 1;
	}
	return 0;
}



#pragma endregion

#pragma region Main Code

int __fastcall hook_luaL_newstate(void ** th, void * edx, char a2, unsigned __int8 a3, int a4)
{
	int ret;
	__PerformanceTest__
	ExecOrig(ret,luaL_newstate, th, a2, a3, a4); //Executing original one first

	lua_State * L = (lua_State *)*th;
	if (L) //Check if state exists
	{
		int top = lua_gettop(L);
		reg_lua("pcall", reluaB_pcall); //Returning pcall back
		reg_lua("dofile", reDofile); //Crash proofed dofile
		reg_lua("logme", reLogMe); //Logging snipet (alt. io.stderr:write(), io.stderr:flush())
		reg_lua("xpcall", reluaB_xpcall); //Old good xpcall
		reg_lua("NewThread", LuaCreateThread); //Creates lua thread
		reg_lua("loadfile", luaB_loadfile); //Returning loadfile

		if (!luaL_loadfile(L, LuaEntryFile)) //1
		{
			if (lua_pcall(L, 0, 1, 0))
			{
				const char * errstr = lua_tolstring(L, top + 1, 0);
				if (errstr)
					LOG( "%s\n", errstr);
			}
		}
		else
		{
			const char * errstr = lua_tolstring(L, top + 1, 0);
			if (errstr)
				LOG("%s\n", errstr);
		}
		lua_settop(L, top); //Clear garbage
	}
	__PerformanceTestEnd__("newstate");
	return ret;
}

char _path[MAX_PATH];
char cfg_path[MAX_PATH];


void init()
{
	__PerformanceTest__
	GetCurrentDirectoryA(MAX_PATH, _path);
	__NormalePath__(_path);
	strcpy(cfg_path, _path);
	strcat(cfg_path, __SettingsFile__);

	if (GetPrivateProfileIntA(INI_MainSection, "enable_console", 0, cfg_path))
	{
		OpenConsole();
		LOG(
"Underground light Lua hook " VERSION "\n"
"Written by: Mary & Jazzman <jazzman@thepiratebay.army>\n"
"This module is licensed under GPLv3.\n"
"See GPLv3.txt for more information.\n"
"Sources can be obtained from here: <https://bitbucket.org/SoulHipHop/underground-light-lua-hook>\n\n"
);
	}

	if (GetPrivateProfileIntA(INI_MainSection, "keep_lightfx", 0, cfg_path))
		InitOriginalLightfx();

	_Bool no_errors =
#ifdef patterns_search_exception
		1;
#endif
	InitLua(); //If some pattern wasn't found, it returns 0

	BeginHooking


	unsigned is_hook_enabled = GetPrivateProfileIntA(INI_MainSection, "hook_enabled", 0, cfg_path);



	if (is_hook_enabled)
	{
		LOG("Hook enabled\n");
#ifndef LuaEntryFile
		GetPrivateProfileStringA(INI_MainSection, "starting_script", "init.lua", LuaEntryFile, sizeof(LuaEntryFile), cfg_path);
#endif
		if (no_errors && luaL_newstate)
			HookFunc(luaL_newstate, hook_luaL_newstate); //Hooking lua newstate method
	}


	FinishHooking
	__PerformanceTestEnd__("Init clock");
}

#pragma endregion

//DLL Entry
_Bool APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		init();

	return 1;
}


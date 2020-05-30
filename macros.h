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
#ifndef MACROSES_INCL
#define MACROSES_INCL
//Differnet macros stored here, including codes macros and etc.

//Settings section
#define VERSION "0.30 EXPERIMENTAL"

#define TestHookFunc
//Sources are compatible with detours aswell. Just setup project with detours and ofcourse you have to compile it aswell.

#if defined TestHookFunc
#undef detour_hooks
#endif

//Define this to don't stop loading dll, when some pattern wasn't found
#define patterns_search_exception

//Hooks through luaL_newstate
#define NewLuaState_Hooking

//Log utility
#define LOG(...) if (ConsoleHandle) fprintf(ConsoleHandle, __VA_ARGS__)

//Performance testing macros
#define CanTestPerformance 0

#if CanTestPerformance
#define __PerformanceTest__ { clock_t ___t___ = clock();

#define __PerformanceTestEnd__( extra ) LOG("(PerfTest) %s %ld\n", extra, clock() - ___t___); }
#else
#define __PerformanceTest__
#define __PerformanceTestEnd__
#endif

//Define this to search only for usable patterns
#define OnlyUsableMethods



//Macro functions

#define OnPatternFound( name, ptr )
//

//Dll export macro
#define OUTTA __declspec(dllexport)


#if defined detour_hooks
	#define BeginHooking	\
		DetourRestoreAfterWith();\
		DetourTransactionBegin();\
		DetourUpdateThread(GetCurrentThread());
	#define HookFunc(origf, newf) DetourAttach(&(PVOID &)origf, newf)
	#define FinishHooking DetourTransactionCommit();
	#define ExecOrig(res, o, ...) res = o(__VA_ARGS__)
#elif defined TestHookFunc
	#define BeginHooking //Does nothing
	#define FinishHooking // Does nothing

	#define HookFunc(origf, newf) hHook_wrap(&st_##origf, origf, newf)
	#define ExecOrig(res, o, ...) hHook_restore(&st_##o); res = o(__VA_ARGS__); hHook__wrap(&st_##o)
#else
#error You forget to define hooking method
#endif


#define __NormalePath__(buff) for (char * pbuff = buff; *pbuff; ++pbuff) { if ( *pbuff == '\\' ) *pbuff = '/'; }

//Patterns for various lua functions
//These are only compatible with PAYDAY 2/PAYDAY: The Heist
//Method to locate newstate in case it broke: just do string search for pcall, xpcall usage and function that uses them is luaL_newstate.
//Don't take long patterns, they aren't gonna add durability. The shorter pattern, the more durability.
#define luaL_newstate_pattern "\x8B\x44\x24\x0C\x56\x8B\xF1\x85"
#define game_tick_pattern "\x8B\x44\x24\x08\x56\x50\x8B\xF1\x8B"
#define lua_pcall_pattern "\x8B\x4C\x24\x10\x83\xEC\x08\x56\x8B\x74\x24\x10"
#define lua_call_pattern "\x8B\x44\x24\x08\x56\x8B\x74\x24\x08\x8B\x56"
#define luaL_loadfile_pattern "\x81\xEC\x20\x02\x00\x00\x55"
#define luaL_loadbuffer_pattern "\x83\xEC\x20\x8B\x44\x24\x28\x8B"
#define lua_load_pattern "\x83\xEC\x20\x8B\x44\x24\x28\x8B\x4C\x24\x2C\x89\x04\x24\x8B\x44"
#define lua_gettop_pattern "\x8B\x4C\x24\x04\x8B\x41\x08\x2B"
#define lua_tolstring_pattern "\x56\x8B\x74\x24\x08\x57\x8B\x7C\x24\x10\x8B\xCF\x8B\xD6\xE8\xED"
#define lua_pushcclosure_pattern "\x56\x8B\x74\x24\x08\x8B\x46\x10\x83\x78\x48\x00\x57\x74\x0C\xFF\x48\x48\x56\xE8\xB8"
#define luaL_register_pattern "\x83\xEC\x08\x53\x8B\x5C\x24\x14\x55\x8B\x6C\x24\x1C"
#define lua_pushboolean_pattern "\x8B\x44\x24\x04\x8B\x48\x08\x33"
#define lua_pushlstring_pattern "\x56\x8B\x74\x24\x08\x8B\x46\x10\x83\x78\x48\x00\x57\x74\x0C\xFF\x48\x48\x56\xE8\xF8"
#define lua_pushstring_pattern "\x8B\x54\x24\x08\x85\xD2\x75\x0F"
#define lua_pushnil_pattern "\x8B\x44\x24\x04\x8B\x48\x08\xC7"

#define lua_setfield_pattern "\x8B\x4C\x24\x08\x83\xEC\x08\x53\x56\x8B\x74\x24\x14\x57\x8B\xD6\
\xE8\x6B\x7C\xFF\xFF\x8B\x54\x24\x20\x8B\xF8\x8B\xC2\x8D\x58\x01\
\x8A\x08\x40\x84\xC9\x75\xF9\x2B\xC3\x50\x52\x56\xE8\x3F\xFB\xFF\
\xFF\x89\x44\x24\x18\x8B\x46\x08\x83\xE8\x08\x50\x8D\x4C\x24\x1C\
\x51\x57\x56\xC7\x44\x24\x2C\x04\x00\x00\x00\xE8\xC0\xC8\xFF\xFF"
#define lua_getfield_pattern "\x8B\x4C\x24\x08\x83\xEC\x08\x53\x56\x8B\x74\x24\x14\x57\x8B\xD6\
\xE8\xCB\x7C\xFF\xFF\x8B\x54\x24\x20\x8B\xF8\x8B\xC2\x8D\x58\x01\
\x8A\x08\x40\x84\xC9\x75\xF9\x2B\xC3\x50\x52\x56\xE8\x9F\xFB\xFF\
\xFF\x89\x44\x24\x18\x8B\x46\x08\x50\x8D\x4C\x24\x1C\x51\x57\x56\
\xC7\x44\x24\x2C\x04\x00\x00\x00\xE8\xD3\xC7\xFF\xFF"

#define lua_settop_pattern "\x8B\x4C\x24\x08\x8B\x44\x24\x04\x85"
#define lua_type_pattern "\x8B\x4C\x24\x08\x8B\x54\x24\x04\xE8\xE3\xFD"
#define lua_insert_pattern "\x8B\x4C\x24\x08\x56\x8B\x74\x24\x08\x8B\xD6\xE8\x50\xFE"
#define lua_pushvalue_pattern "\x8B\x4C\x24\x08\x56\x8B\x74\x24\x08\x8B\xD6\xE8\x10\xFE"
#define lua_replace_pattern "\x56\x8B\x74\x24\x08\x57\x8B\x7C\x24\x10\x81"
#define lua_remove_pattern "\x8B\x4C\x24\x08\x56\x8B\x74\x24\x08\x8B\xD6\xE8\x90"
#define lua_newuserdata_pattern "\x56\x8B\x74\x24\x08\x8B\x46\x10\x83\x78\x48\x00\x74\x0C\xFF\x48\x48\x56\xE8\x79"
#define lua_newthread_pattern "\x56\x8B\x74\x24\x08\x8B\x46\x10\x83\x78\x48\x00\x74\x0C\xFF\x48\x48\x56\xE8\x39"
#define lua_touserdata_pattern "\x8B\x4C\x24\x08\x8B\x54\x24\x04\xE8\x13"
#define lua_toboolean_pattern "\x8B\x4C\x24\x08\x8B\x54\x24\x04\xE8\x43"

//#define ClueNames(p1,p2) ##p1##p2
#define GetPattern(ptr, inf) FindPattern(ptr, sizeof(ptr)-1, inf)
//#define _GetPattern(str, ptr, inf) ptr = ( T##ptr )GetPattern( ##ptr##_pattern, inf ); OnPatternFound(str, ptr)
#define _GetPattern(str, ptr, ptrn, inf) ptr = GetPattern( ptrn, inf ); OnPatternFound(str, ptr)




#define __SettingsFile__ "/light_hook.ini"
#define INI_MainSection "main"

//#define LuaEntryFile "init.lua"
#endif
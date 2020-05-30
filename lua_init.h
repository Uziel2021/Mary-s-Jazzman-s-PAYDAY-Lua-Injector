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

//Lua stuff
#ifndef LUA_INIT_INCL
#define LUA_INIT_INCL

#include "main.h"
#include "tools.h"


/*---------------------------------------Lua Macros----------------------------------------------------*/
//Some of the code taken from here: http://www.lua.org/source/5.1/

/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET     (-1)

/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX       (-10000)
#define LUA_ENVIRONINDEX        (-10001)
#define LUA_GLOBALSINDEX        (-10002)
#define lua_upvalueindex(i)     (LUA_GLOBALSINDEX-(i))

/*
** basic types
*/
#define LUA_TNONE               (-1)

#define LUA_TNIL                0
#define LUA_TBOOLEAN            1
#define LUA_TLIGHTUSERDATA      2
#define LUA_TNUMBER             3
#define LUA_TSTRING             4
#define LUA_TTABLE              5
#define LUA_TFUNCTION           6
#define LUA_TUSERDATA           7
#define LUA_TTHREAD             8

/* Status and error codes */

/* thread status; 0 is OK */
#define LUA_YIELD       1
#define LUA_ERRRUN      2
#define LUA_ERRSYNTAX   3
#define LUA_ERRMEM      4
#define LUA_ERRERR      5

/* extra error code for `luaL_load' */
#define LUA_ERRFILE     (LUA_ERRERR+1)

/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)            lua_settop(L, -(n)-1)

#define lua_newtable(L)         lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)  lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)         lua_objlen(L, (i))

#define lua_isfunction(L,n)     (lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)        (lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)        (lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)          (lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)      (lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)       (lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)         (lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)   (lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)   \
	lua_pushlstring(L, "" s, (sizeof(s) / sizeof(char)) - 1)

#define lua_setglobal(L,s)      lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)      lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)       lua_tolstring(L, (i), NULL)


typedef void lua_State;
//

	typedef int(*lua_CFunction) (lua_State * L);

	typedef struct luaL_Reg {
		const char *name;
		lua_CFunction func;
	} luaL_Reg;

	//Non official macros
#define reg_lua(fname, f) lua_pushcclosure(L, f, 0); lua_setglobal(L, fname)


	/*-----------------------------------------------------------------------------------------------------*/

	//Patterns and stuff
	//Lua typedefs

	typedef int(__thiscall * TluaL_newstate)(void *th, char a2, unsigned __int8 a3, int a4); //luaL_newstate
	typedef int(__cdecl * TluaL_loadfile)(lua_State * L, const char * file_path); //luaL_loadfile
	typedef int(__cdecl * TluaL_loadbuffer)(lua_State *L, const char *buff, size_t size,const char *name); //luaL_loadbuffer
	typedef void(__cdecl * Tlua_call)(lua_State *L, int nargs, int nresults); //lua_call
	typedef int(__cdecl* Tlua_pcall)(lua_State * L, int nargs, int nresults, int errfunc); //lua_pcall
	typedef const char * (__cdecl* Tlua_tolstring)(lua_State * L, int index, size_t *len); //lua_tolstring
	typedef int(__cdecl * Tlua_toboolean)(lua_State *L, int idx); //lua_toboolean
	typedef int(__thiscall * TOnGameTick)(void * th, int a2, const char * field); //OnGameTick
	typedef int(__cdecl * Tlua_gettop)(lua_State * L); //lua_gettop
	typedef void(__cdecl * Tlua_pushcclosure)(lua_State * L, lua_CFunction fn, int n); //lua_pushcclosure
	typedef void(__cdecl * Tlua_pushlstring)(lua_State * L, const char *s, size_t len); //lua_pushlstring
	typedef void(__cdecl * Tlua_pushstring)(lua_State * L, const char *s); //lua_pushstring
	typedef void(__cdecl * Tlua_pushnil)(lua_State *L); //lua_pushnil
	typedef void(__cdecl * Tlua_setfield)(lua_State * L, int index, const char *k); //lua_setfield
	typedef void(__cdecl * Tlua_getfield)(lua_State * L, int index, const char *k); //lua_getfield
	typedef void(__cdecl * Tlua_settop)(lua_State * L, int index); //lua_settop
	typedef int(__cdecl * Tlua_type)(lua_State * L, int index); //lua_type
	typedef void(__cdecl * TluaL_register)(lua_State * L, const char *libname, const luaL_Reg *l);
	typedef void(__cdecl * Tlua_pushboolean)(lua_State * L, int b);
	typedef void(__cdecl * Tlua_insert)(lua_State * L, int index);
	typedef void(__cdecl * Tlua_replace)(lua_State * L, int index);
	typedef void(__cdecl * Tlua_pushvalue)(lua_State *L, int idx);
	typedef void(__cdecl * Tlua_remove)(lua_State *L, int index);
	typedef lua_State *(__cdecl * Tlua_newthread)(lua_State *L);




	//Main and init code
	TluaL_newstate luaL_newstate = NULL;
	Tlua_call lua_call = NULL;
	Tlua_pcall lua_pcall = NULL;
	TluaL_loadfile luaL_loadfile = NULL;
	Tlua_tolstring lua_tolstring = NULL;
	Tlua_toboolean lua_toboolean = NULL;
	Tlua_gettop lua_gettop = NULL;
	Tlua_pushcclosure lua_pushcclosure = NULL;
	Tlua_getfield lua_getfield = NULL;
	Tlua_setfield lua_setfield = NULL;
	Tlua_settop lua_settop = NULL;
	Tlua_type lua_type = NULL;
	Tlua_replace lua_replace = NULL;
	Tlua_pushboolean lua_pushboolean = NULL;
	Tlua_insert lua_insert = NULL;
	Tlua_pushstring lua_pushstring = NULL;
	Tlua_pushlstring lua_pushlstring = NULL;
	Tlua_pushnil lua_pushnil = NULL;
	Tlua_newthread lua_newthread = NULL;


#ifndef OnlyUsableMethods
	TluaL_loadbuffer luaL_loadbuffer = NULL;
	TOnGameTick game_tick = NULL;
	Tlua_pushvalue lua_pushvalue = NULL;
	Tlua_remove lua_remove = NULL;
#endif
	_Bool InitLua()
	{
		struct AddrInfo inf = GetAddrInfoByModule(NULL);
		if (inf.start) //Will be NULL if somehow it not found module of process we are attached to.
		{
			_GetPattern("luaL_newstate   ", luaL_newstate, luaL_newstate_pattern, &inf);
			_GetPattern("lua_call        ", lua_call, lua_call_pattern, &inf);
			_GetPattern("lua_pcall       ", lua_pcall, lua_pcall_pattern, &inf);
			_GetPattern("luaL_loadfile   ", luaL_loadfile, luaL_loadfile_pattern, &inf);
			_GetPattern("lua_tolstring   ", lua_tolstring, lua_tolstring_pattern, &inf);
			_GetPattern("lua_toboolean   ", lua_toboolean, lua_toboolean_pattern, &inf);
			_GetPattern("lua_gettop      ", lua_gettop, lua_gettop_pattern, &inf);
			_GetPattern("lua_pushcclosure", lua_pushcclosure, lua_pushcclosure_pattern, &inf);
			_GetPattern("lua_getfield    ", lua_getfield, lua_getfield_pattern, &inf);
			_GetPattern("lua_setfield    ", lua_setfield, lua_setfield_pattern, &inf);
			_GetPattern("lua_settop      ", lua_settop, lua_settop_pattern, &inf);
			_GetPattern("lua_type        ", lua_type, lua_type_pattern, &inf);
			_GetPattern("lua_pushboolean ", lua_pushboolean, lua_pushboolean_pattern, &inf);
			_GetPattern("lua_insert      ", lua_insert, lua_insert_pattern, &inf);
			_GetPattern("lua_pushlstring ", lua_pushlstring, lua_pushlstring_pattern, &inf);
			_GetPattern("lua_pushstring  ", lua_pushstring, lua_pushstring_pattern, &inf);
			_GetPattern("lua_pushnil     ", lua_pushnil, lua_pushnil_pattern, &inf);
			_GetPattern("lua_newthread   ", lua_newthread, lua_newthread_pattern, &inf);
			_GetPattern("lua_replace     ", lua_replace, lua_replace_pattern, &inf);


#ifndef OnlyUsableMethods
			_GetPattern("luaL_loadbuffer ", luaL_loadbuffer, luaL_loadbuffer_pattern, &inf);
			game_tick = (TOnGameTick)GetPattern(game_tick_pattern, &inf);
			OnPatternFound("game_tick       ", game_tick);
			_GetPattern("lua_pushvalue   ", lua_pushvalue, lua_pushvalue_pattern, &inf);
			_GetPattern("lua_remove      ", lua_remove, lua_remove_pattern, &inf);
#endif
			if (luaL_newstate &&
				lua_pcall &&
				luaL_loadfile &&
				lua_tolstring &&
				lua_gettop &&
				lua_pushcclosure &&
				lua_getfield &&
				lua_setfield &&
				lua_settop &&
				lua_type &&
				lua_pushboolean &&
				lua_pushnil &&
				lua_newthread &&
				lua_pushstring
				)
				return 1;
#ifndef patterns_search_exception
			LOG("Pattern search failed!\n");
#endif
		}
		return 0;
	};
#ifndef OnlyUsableMethods
	int luaL_loadstring(lua_State *L, const char *s)
	{
		return luaL_loadbuffer(L, s, strlen(s), s);
	};
#endif
#endif
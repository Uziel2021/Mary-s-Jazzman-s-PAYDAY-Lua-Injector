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
#ifndef TOOLS_INCL
#define TOOLS_INCL

#ifdef TestHookFunc

//Jump on ptr (0xE9)
#define OP_JMP 0xE9

//RET (0xC3)
#define OP_RET 0xC3

//NOP (0x90)
#define OP_NOP 0x90

//Size of final opcode
#define jmp_size 5

#define DBG_OP( op, size ) for(unsigned short I = 0; I < size; I++) LOG(" %hX ", unsigned short((char *)(op)[I]));

//Oldschool uneffective hooking method, not for hooking loops.
//Whoever don't like it, use detours
struct hHook
{
	char _old_bytes[jmp_size];
	char op[jmp_size];
	void * loc;
	DWORD olProtection;
};

void hHook__wrap(struct hHook *in);
void hHook_restore(struct hHook *in);
void hHook_wrap(struct hHook *in, void * orig, void * newf);

#endif

struct AddrInfo
{
	void *start, *end;
};

//Gets start&end pointer for some "modname"
struct AddrInfo GetAddrInfoByModule(const char *modname);

//Flag, if we still use C style fopen_s
#define C_fopen

//Method helper that will create folders, needed for filename to write something
void CreateMissingFolders(const char * path);

//Reads from file
_Bool ReadFromFile(const char * path, char * data, size_t * size);

//Write to file
//Use: (string)path, (char *)data, (size_t)size
_Bool WriteToFile(const char * path, const char * data, size_t size, _Bool append);

//Pattern search with mask. "x" = exact match, "?" = can be any
//Use: (string pattern, string mask, string module_name)
void* FindPatternMask(const char*, const char*, const char*);

//Pattern search, but if mask isn't needed or contains only XXXX
//Use: (string pattern, string module_name)
void* FindPattern(const char*,const unsigned,const struct AddrInfo*);
#endif
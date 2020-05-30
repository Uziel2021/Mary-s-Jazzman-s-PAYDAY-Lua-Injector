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

#include "main.h"
#include "tools.h"
#include "Console.h"
//#include <fstream>



#ifdef TestHookFunc

void hHook__wrap(struct hHook *in)
{
	void * orig = in->loc;
	VirtualProtect(orig, jmp_size, PAGE_EXECUTE_READWRITE, NULL);
	memcpy(orig, in->op, sizeof(in->op));
	VirtualProtect(orig, jmp_size, in->olProtection, NULL);
}
void hHook_restore(struct hHook *in)
{
	void * orig = in->loc;
	VirtualProtect(orig, jmp_size, PAGE_EXECUTE_READWRITE, NULL);
	memcpy(orig, in->_old_bytes, sizeof(in->_old_bytes));
	VirtualProtect(orig, jmp_size, in->olProtection, NULL);
}
void hHook_wrap(struct hHook *in, void * orig, void * newf)
{
	//fprintf_s(stderr, "O: %p, N: %p\n", orig, newf);
	int dist = (int)newf - (int)(orig + 5);
	in->op[0] = OP_JMP;
	*((int *)((char *)in->op + 1)) = dist;
	VirtualProtect(orig, jmp_size, PAGE_EXECUTE_READWRITE, &in->olProtection);
	memcpy(in->_old_bytes, orig, sizeof(in->_old_bytes));
	memcpy(orig, in->op, sizeof(in->op));
	VirtualProtect(orig, jmp_size, in->olProtection, NULL);
	in->loc = orig;
	/*while (1)
	Sleep(1);*/
}
#endif

//Method helper that will create folders, needed for filename to write something
void CreateMissingFolders(const char * path)
{
	char out[MAX_PATH];
	memset(out, 0, MAX_PATH);
	for (char * pout = out; *path; path++, pout++)
	{
		char c = *path;
		if (c == '\\' || c == '/')
		{
			CreateDirectoryA(out, NULL);
			*pout = c;
		}
		else if (c == ':')
		{
			//Drive letter found, skip it
			*pout = c;
			*(++pout) = *(++path);
		}
		else
			*pout = c;
	}
}

_Bool ReadFromFile(const char * path, char * data, size_t * size)
{
	_Bool success = 0;
	FILE * file;
	file = fopen( path, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		*size = ftell(file);
		data = (char*)malloc(*size);
		fseek(file, 0, SEEK_SET);
		fread(data, sizeof(char), *size, file);
		fclose(file);
		success=1;
	}
	return success;
}

_Bool WriteToFile(const char * path, const char * data, size_t size, _Bool append)
{
	FILE * file;
	file = fopen(path, append ? "ab" : "wb");
	if (file)
	{
		fwrite(data, sizeof(char), size, file);
		fclose(file);
		return 1;
	}
	return 0;
}

struct AddrInfo GetAddrInfoByModule(const char *modname)
{
	char* proc = (char*)GetModuleHandleA(modname);
	struct AddrInfo res = { NULL, NULL };
	if (proc)
	{
		PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)proc;
		if (DOSHeader->e_magic == IMAGE_DOS_SIGNATURE)
		{
			IMAGE_OPTIONAL_HEADER32 optHeader = ((PIMAGE_NT_HEADERS32)(proc + DOSHeader->e_lfanew))->OptionalHeader;
			res.start = proc;
			res.end = proc + optHeader.SizeOfCode;
		}
	}
	return res;
}

//Pattern search

void * FindPatternMask(const char* strPattern, const char* strMask, const char* module)
{
	char* ModuleBase = NULL;
	//Get module base address or process base address if no module is passed
	if (module)
		ModuleBase = (char*)GetModuleHandleA(module);
	else
		ModuleBase = (char*)GetModuleHandleA(NULL);

	//Couldn't find module
	if (ModuleBase)
	{
		//Grab DOS header
		PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)ModuleBase;

		//Verify DOS header
		if (pDOSHeader->e_magic == IMAGE_DOS_SIGNATURE)
		{
			//Grab NT header
			IMAGE_OPTIONAL_HEADER32 optHeader = ((PIMAGE_NT_HEADERS32)(ModuleBase + pDOSHeader->e_lfanew))->OptionalHeader;

			//Calculate where the code section starts, as that's where we'll be scanning for signatures
			ModuleBase += optHeader.BaseOfCode;
			unsigned ModuleSize = optHeader.SizeOfCode;
			unsigned maskLen = strlen(strMask);

			//Calculate end of code section
			char* Final = (ModuleBase + ModuleSize) - maskLen;

			for (unsigned BytePos = 0; ModuleBase < Final; ++ModuleBase)
			{
				if (strMask[BytePos] == 'x')
				{
					if (strPattern[BytePos] == *ModuleBase)
						++BytePos;
					else
					{
						ModuleBase -= BytePos - 1;
						BytePos = 0;
					}

				}
				else
					BytePos++;
				if (BytePos == maskLen)
					return ModuleBase - BytePos + 1;
			}
		}
	}

	return NULL;
}

void * FindPattern(const char* strPattern,const unsigned ptrnLen,const struct AddrInfo *inf)
{
	char* Final = (char*)inf->end - ptrnLen;
	char* Base = (char*)inf->start;
	for (unsigned BytePos = 0; Base < Final; ++Base)
	{
		if (strPattern[BytePos] == *Base)
		{
			if (++BytePos == ptrnLen)
				return Base - BytePos + 1;
		}
		else
		{
			Base -= BytePos - 1;
			BytePos = 0;
		}
	}

	return NULL;
}
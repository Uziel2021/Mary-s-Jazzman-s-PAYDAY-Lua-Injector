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

#ifndef LIGHTFX_INCL
#define LIGHTFX_INCL

typedef BOOL(__cdecl * P_Initialize)(void);
typedef BOOL(__cdecl * P_Reset)(void);
typedef BOOL(__cdecl * P_Light)(int, int);
typedef BOOL(__cdecl * P_Update)(void);
typedef BOOL(__cdecl * P_Release)(void);
typedef BOOL(__cdecl * P_GetDeviceDescription)(int, char*, size_t, int*);

P_Initialize M_Initialize;
P_Reset M_Reset;
P_Light M_Light;
P_Update M_Update;
P_Release M_Release;
P_GetDeviceDescription M_GetDeviceDescription;

void InitOriginalLightfx()
{
	char dir[MAX_PATH];
	GetSystemDirectory(dir, MAX_PATH);
	strcat(dir, "/LightFX.dll");
	HMODULE lib = LoadLibrary(dir); //Try to load from system folder
	if (!lib)
	{
		GetWindowsDirectory(dir, MAX_PATH);
		strcat(dir, "/LightFX.dll");
		lib = LoadLibrary(dir); //From windows folder
		#if 0
		if (!lib)
		{
			GetSystemWow64Directory(dir, MAX_PATH);
			strcat(dir, "/LightFX.dll");
			lib = LoadLibrary(dir); //From SysWow64
		}
		#endif
	}
	if (lib && lib != GetModuleHandle("LightFX.dll")/*Shoudn't happen really*/)
	{
		M_Initialize = (P_Initialize)GetProcAddress(lib, "LFX_Initialize");
		M_Reset = (P_Reset)GetProcAddress(lib, "LFX_Reset");
		M_Light = (P_Light)GetProcAddress(lib, "LFX_Light");
		M_Update = (P_Update)GetProcAddress(lib, "LFX_Update");
		M_Release = (P_Release)GetProcAddress(lib, "LFX_Release");
		LOG("Initialized original LightFX, handle %p\n\n", lib);
	}
}

//Some of stuff taken from alienware SDK
OUTTA BOOL __cdecl LFX_Initialize()
{
	if (M_Initialize)
		return M_Initialize();
	return FALSE;
}
OUTTA BOOL __cdecl LFX_Reset()
{
	if (M_Reset)
		return M_Reset();
	return FALSE;
}
OUTTA BOOL __cdecl LFX_Light(int c1, int c2)
{
	if (M_Light)
		return M_Light(c1, c2);
	return FALSE;
}
OUTTA BOOL __cdecl LFX_Update()
{
	if (M_Update)
		return M_Update();
	return FALSE;
}
OUTTA BOOL __cdecl LFX_Release()
{
	if (M_Release)
		return M_Release();
	return FALSE;
}
#endif
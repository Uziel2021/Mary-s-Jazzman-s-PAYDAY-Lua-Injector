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

FILE * ConsoleHandle;

void OpenConsole()
{
	//ConsoleHandle=stdout;
	//To make console work on GNU/Linux, you have to comment whole code below and uncomment just this line above.
	if (AllocConsole())
	{
		ConsoleHandle = freopen("CONOUT$", "wt", stderr);
		if (ConsoleHandle)
		{
			SetConsoleTitleA("Console");
			SetConsoleActiveScreenBuffer(ConsoleHandle);
		}
	}
}

#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <windows.h>
#include <string>
/**
* C++ Help Class
*/
class Help
{
	public:
	//Help();
	static std::string GetHelpTxt();
};

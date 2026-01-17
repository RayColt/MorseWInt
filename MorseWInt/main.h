#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include <windows.h>
#include "resource.h"
#include <random>
#include <string>
#include "morse.h"
#include "help.h"
#include "morsewav.h"
// GUI includes
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

using namespace std;

static void MakeMorseSafe(Morse& morse);
int get_options(int argc, char* argv[]);
string arg_string(char* arg);

wstring StringToWString(const string& str);
string WStringToString(const wstring& wstr);
wstring GetTextFromEditField(HWND hWnd);
string trimDecimals(const std::string& s, int decimals);
wstring StringToWString(const string& str);
double wstring_to_double(const std::wstring& s);
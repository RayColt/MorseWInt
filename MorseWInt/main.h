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
#include <vector>
#include <thread>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <process.h> // for _beginthreadex
#include <thread>

#define WM_MWAV_DONE (WM_USER + 1)

// GUI includes
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

struct WavThreadParams {
    std::string morse;
    double tone;
    double wpm;
    double sps;
    int channels;
    bool openExternal;
    HWND hwnd;
};

struct WavThreadResult {
    std::wstring fullPath;
    double tone;
    int wpm;
    int sps;
    size_t waveSize;
    size_t pcmCount;
    int channels;
};

using namespace std;

int get_options(int argc, char* argv[]);
string arg_string(char* arg);

wstring GetTextFromEditField(HWND hWnd);
string trimDecimals(const std::string& s, int decimals);
wstring StringToWString(const string& str);
double wstring_to_double(const std::wstring& s);

wstring StringToWString(const string& str);
string WStringToString(const wstring& wstr);
int ParseIntFromEdit(HWND hEdit, int defaultVal);
double ParseDoubleFromEdit(HWND hEdit, double defaultVal);


static LRESULT CALLBACK Edit_SelectAll_SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

static LRESULT CALLBACK MorseWIntWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


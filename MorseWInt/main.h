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
#include <tchar.h>
#include <commdlg.h>
#include <mmsystem.h>
#include <ctype.h>
#include <strsafe.h>

#define WM_MWAV_DONE (WM_USER + 1)

#define IDM_FILE_OPEN       1000
#define IDM_PLAY_PLAY       1001
#define IDM_PLAY_PAUSE      1002
#define IDM_PLAY_STOP       1003
#define IDM_SLIDER_UPDATE   1004
#define SLIDER_TIMER_MS     1005


// GUI includes
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
// Multimedia library
#pragma comment(lib, "winmm.lib")

struct WavThreadParams {
    std::string morse;
    double tone;
    double wpm;
    double sps;
    int channels;
    bool openExternal;
	bool showExternal;
	bool saveDirOk;
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

struct ConsoleWavParams
{
    std::string morse;
    double tone;
    double wpm;
    double sps;
    int channels;
    bool openExternal;
	bool showExternal;
};

using namespace std;
bool SaveDirOk = false;   // directory creation status
std::string FullPath = ""; // full path to save file

int get_options(int argc, char* argv[]);
string arg_string(char* arg);

wstring GetTextFromEditField(HWND hWnd);
string trimDecimals(const std::string& s, int decimals);

wstring StringToWString(const string& str);
string WStringToString(const wstring& wstr);

int ParseIntFromEdit(HWND hEdit, int defaultVal);
double ParseDoubleFromEdit(HWND hEdit, double defaultVal);

static LRESULT CALLBACK Edit_SelectAll_SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

static LRESULT CALLBACK MorseWIntWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ClosePlayer();
void PlayMedia(HWND hWndParent);
void PauseMedia();
void ResumeMedia();
void StopMedia();
static bool QueryMode(std::wstring& mode);
static bool QueryLength(UINT& lengthMs);
static bool QueryPosition(UINT& posMs);
void SetTracker(HWND hTrackbar);
void ShowMciError(MCIERROR err, HWND hWnd, LPCTSTR prefix);
BOOL InitWavPlayerWindow(HWND hWndParent); // create hidden child window to get a HWND for MCI notifications 
static MCIERROR OpenMediaFileAndPlay(const std::wstring& path, HWND hWndParent);

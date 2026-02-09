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
#define SLIDER_TIMER_MS     200

// GUI includes
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
// Multimedia library
#pragma comment(lib, "winmm.lib")

// ------------------ Global Variables ----------------
using namespace std;
bool SaveDirOk = false;   // directory creation status
Morse m; // global morse settings
string action = ""; // global action setting
std::string FullPath = ""; // full path to save file
TCHAR g_szMediaFile[MAX_PATH] = { 0 };
bool g_mediaOpen = false;

// config options
bool NEW_CONSOLE = false; // open a new console for output, false for recommended cmd.exe, true for powershell - buggy
const bool OPEN_EXTERNAL_MEDIAPLAYER = true; // play sound with external media player or not - CONSOLE MODUS ONLY
const bool SHOW_EXTERNAL_MEDIAPLAYER = true; // play sound with visible external media player or not - CONSOLE MODUS ONLY

// -------------------- Global Window Handles ----------------
HWND g_hWnd = NULL; // global window handle
HWND g_hWndPlayer = NULL; // global media player window handle
static HINSTANCE g_hInst = GetModuleHandle(nullptr); // global instance handle 

// input limits
const int MAX_TXT_INPUT = 3000; // max chars for morse encoding/decoding
const int MAX_MORSE_INPUT = 3000; // max chars for morse encoding/decoding
const int MAX_SOUND_INPUT = 2999; // max chars for sound generation
const int MONO = 1;
const int STEREO = 2;

// default morse settings
const string error_in = "INPUT-ERROR";
double frequency_in_hertz = 880.0;
int words_per_minute = 33;
int samples_per_second = 44100;

// ----------------- MorseWInt Data Structures ----------------
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

// ---------------- MorseWInt Helper Functions ----------------
int get_options(int argc, char* argv[]);
string arg_string(char* arg);

wstring GetTextFromEditField(HWND hWnd);
string trimDecimals(const std::string& s, int decimals);

wstring StringToWString(const string& str);
string WStringToString(const wstring& wstr);

int ParseIntFromEdit(HWND hEdit, int defaultVal);
double ParseDoubleFromEdit(HWND hEdit, double defaultVal);

// ---------------- MorseWInt Media Player Functions ----------------
void ClosePlayer();
void PlayMedia();
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


// ---------------- MorseWInt GUI ----------------
enum
{
    CID_ENCODE = 100, CID_DECODE = 101, CID_EDIT = 102, CID_MORSE = 103, CID_BIN = 104,
    CID_HEX = 105, CID_HEXBIN = 106, CID_M2WS = 107, CID_M2WM = 108, CID_WAVOUT = 109, CID_HELP = 110,
    CID_TONE = 111, CID_WPM = 112, CID_SPS = 113, CID_PROG = 114, CID_PLAY = 115, CID_PAUSE = 116, CID_STOP = 117,
    CID_TRACK = 118
};

// Global handles to child controls
HWND hEdit = NULL;
HWND hWavOut = NULL;
HWND hTone = NULL;
HWND hWpm = NULL;
HWND hSps = NULL;
HWND hProg = NULL;
HWND hCountLabel = NULL;
HWND hPlay = NULL;
HWND hPause = NULL;
HWND hStop = NULL;
HWND g_hTrack = NULL;

// ----------------- Procedures -----------------
static LRESULT CALLBACK Edit_SelectAll_SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

static LRESULT CALLBACK MorseWIntWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


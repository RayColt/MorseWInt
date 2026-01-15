/**
* MorseWInt.cpp 
* 
* International Morse Code Encoder/Decoder/Wav creator with GUI and Command Line Interface in One!
* 
* @author RAY COLT
* @version 007
*/
#include <windows.h>
#include <random>
#include <string>
#include "morse.h"
#include "help.h"
#include "morsewav.h"
// GUI includes
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

using namespace std;

HWND g_hWnd = NULL; // global window handle
static HINSTANCE g_hInst = NULL; // global instance handle
Morse m; // global morse settings
string action = ""; // global action setting

// set to true to open new output console window - less buggy
// to false for what should have been
const bool NEW_CONSOLE = true; 

const int MAX_TXT_INPUT = 6000; // max chars for morse encoding/decoding
const int MAX_MORSE_INPUT = 2000; // max chars for morse encoding/decoding
const int MAX_SOUND_INPUT = 750; // max chars for sound generation

/**
* Create Safe morse settings
*/
static void MakeMorseSafe(Morse& morse)
{
    if (morse.samples_per_second < 8000.0) morse.samples_per_second = 8000.0;
    if (morse.samples_per_second > 48000) morse.samples_per_second = 48000.0;
    if (morse.frequency_in_hertz < 20.0) morse.frequency_in_hertz = 20.0;
    if (morse.frequency_in_hertz > 8000.0) morse.frequency_in_hertz = 8000.0;
    if (morse.words_per_minute < 0.0) morse.words_per_minute = 0.0;
    if (morse.words_per_minute > 50.0) morse.words_per_minute = 50.0;
}

/**
* Read cmd line user arguments
*
* @param argc
* @param argv[]
* @return int
*/
int get_options(int argc, char* argv[])
{
    int args = 0;
    bool ok = false;
    // check for valid mode
    if (strncmp(argv[1], "e", 1) == 0 || strncmp(argv[1], "b", 1) == 0 || strncmp(argv[1], "d", 1) == 0 ||
        strncmp(argv[1], "he", 2) == 0 || strncmp(argv[1], "hd", 2) == 0 || strncmp(argv[1], "hb", 2) == 0 ||
        strncmp(argv[1], "hbd", 3) == 0)
    {
        ok = true;
    }
    if (strncmp(argv[1], "-help", 5) == 0 || strncmp(argv[1], "-h", 2) == 0)
    {
        cout << Help::GetHelpTxt();
        ok = true;
    }
	else if (ok) // read sound mode settings
    {
        while (argc > 1)
        {
            if (strncmp(argv[2], "-hz:", 4) == 0)
            {
                m.frequency_in_hertz = atof(&argv[2][4]);
            }
            else if (strncmp(argv[2], "-wpm:", 5) == 0)
            {
                m.words_per_minute = atof(&argv[2][5]);
            }
            else if (strncmp(argv[2], "-sps:", 5) == 0)
            {
                m.samples_per_second = atof(&argv[2][5]);
            }
            else
            {
                break;
            }
            argc -= 1;
            argv += 1;
            args += 1;
        }
    }
    else
    {
        fprintf(stderr, "option error %s, see morse -help for info\n", argv[2]);
        exit(1);
    }
    return args;
}

/**
* Generate string from arguments
* @param arg
*
* @return string
*/
string arg_string(char* arg)
{
    char c; string str;
    while ((c = *arg++) != '\0')
    {
        str += c;
    }
    str += " ";
    return str;
}

/**
* Creates new output console
*  or attaches to parent console - buggy
* 
* @param newconsole
*/
static void AttachToConsole(BOOLEAN newconsole)
{
	if (newconsole) AllocConsole();
	else AttachConsole(ATTACH_PARENT_PROCESS);

    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

// ---------------- MorseWInt GUI ----------------

enum 
{
	CID_ENCODE = 100, CID_DECODE = 101, CID_EDIT = 102, CID_MORSE = 103, CID_BIN = 104, 
    CID_HEX = 105, CID_HEXBIN = 106, CID_M2WS = 107, CID_M2WM = 108, CID_WAVOUT = 109 
};

// Create child controls on given window
static void CreateMorseControls(HWND hWnd)
{
    WNDCLASSEX wc;
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
	// radiobutton x position
    int radiobuttonX = 425;
    int radiobuttonY = 45;

    // create ms font
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hFontBold = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
	
    // create labels
    HWND hMorseLabel = CreateWindowExW(0, L"STATIC", L"MORSE or TXT:", 
        WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 120, 18, 
        hWnd, NULL, g_hInst, NULL);

    // create labels
    HWND hModesLabel = CreateWindowExW(0, L"STATIC", L"MODES:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, 425, 25, 120, 18,
        hWnd, NULL, g_hInst, NULL);
	 
    HWND hWavLabel = CreateWindowExW(0, L"STATIC", L"WAV OUTPUT:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX, 200, 120, 18,
        hWnd, NULL, g_hInst, NULL);
	
    // create edit box
    HWND hEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN |
        WS_VSCROLL,
        15, 40, 400, 300,
        hWnd,
        (HMENU)CID_EDIT,
        g_hInst,
        NULL
    );

    // create wav output edit box
    HWND hWavOut = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", 
        NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE, 
        radiobuttonX, 220, 200, 120, 
        hWnd, (HMENU)CID_WAVOUT, g_hInst, NULL);

	// create radio buttons
    HWND hMorse = CreateWindowExW(
        WS_EX_TRANSPARENT, L"BUTTON", L"Morse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY, 165, 20,
        hWnd, (HMENU)CID_MORSE, g_hInst, NULL
    );

    HWND hBinMorse = CreateWindowExW(
        WS_EX_TRANSPARENT, L"BUTTON", L"BinMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY + 25, 165, 20,
        hWnd, (HMENU)CID_BIN, g_hInst, NULL
    );

    HWND hHexMorse = CreateWindowExW(
        0, L"BUTTON", L"HexMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY + 50, 165, 20,
        hWnd, (HMENU)CID_HEX, g_hInst, NULL
    );

    HWND hHexBinMorse = CreateWindowExW(
        0, L"BUTTON", L"HexBinMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY + 75, 165, 20,
        hWnd, (HMENU)CID_HEXBIN, g_hInst, NULL
    );

    HWND hMorseToWavS = CreateWindowExW(
        0, L"BUTTON", L"Morse to Wav(s)",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY + 100, 165, 20,
        hWnd, (HMENU)CID_M2WS, g_hInst, NULL
    );

    HWND hMorseToWavM = CreateWindowExW(
        0, L"BUTTON", L"Morse to Wav(m)",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, radiobuttonY + 125, 165, 20,
        hWnd, (HMENU)CID_M2WM, g_hInst, NULL
    );

    // create progress bar
    HWND hProg = CreateWindowExW(
        0,
        PROGRESS_CLASSW,
        NULL,
        WS_CHILD | WS_VISIBLE,
        270, 15, 144, 20,
        hWnd,
        (HMENU)2001,
        g_hInst,
        NULL
    );

	// Initialize progress bar
    SendMessageW(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, 100));   // 0–100%
    SendMessageW(hProg, PBM_SETPOS, 0, 0);                      // start at 0
	SendMessageW(hProg, PBM_SETPOS, 33, 0); // update position %
	//SendMessageW(hProg, PBM_STEPIT, 0, 0); // increment position by step amount
    //SendMessageW(hProg, PBM_SETBKCOLOR, 0, RGB(0, 0, 0));        // background
    //SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(255, 255, 255)); // bar color

	// create buttons
    HWND hEncodeButton = CreateWindowExW(0, L"BUTTON", L"ENCODE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 15, 355, 185, 40, hWnd, (HMENU)CID_ENCODE, g_hInst, NULL);
    HWND hDecodeButton = CreateWindowExW(0, L"BUTTON", L"DECODE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 225, 355, 185, 40, hWnd, (HMENU)CID_DECODE, g_hInst, NULL);

	// set fonts
	// labels
    SendMessageW(hMorseLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
	SendMessageW(hModesLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hWavLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    // edit box
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWavOut, WM_SETFONT, (WPARAM)hFont, TRUE);
    // buttons
    SendMessageW(hEncodeButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hDecodeButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
	// radio buttons
    SendMessageW(hMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
	SendMessageW(hBinMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hHexMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hHexBinMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hMorseToWavS, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hMorseToWavM, WM_SETFONT, (WPARAM)hFont, TRUE);
	
}

// Morse window proc handles control actions and closes window
LRESULT CALLBACK MorseWIntWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            CreateMorseControls(hWnd);
            return 0;
        }
        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            if (id == CID_ENCODE)
            {
                DestroyWindow(hWnd);
                return 0;
            }
            else if (id == CID_DECODE)
            {
                DestroyWindow(hWnd);
                return 0;
            }
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(g_hWnd, &ps);

            // Fill background with custom color
            HBRUSH bg = CreateSolidBrush(RGB(240, 240, 240));
            FillRect(hdc, &ps.rcPaint, bg);
            DeleteObject(bg);

            EndPaint(g_hWnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProcW(hWnd, msg, wParam, lParam);
        }
        return 0;
}

static int ShowMorseApp(HWND &hwnd)
{
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = MorseWIntWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"MorseWIntWindowClass";

    RegisterClass(&wc);

    hwnd = CreateWindow(
        wc.lpszClassName,
        L"MorseWInt 007",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 500,
        nullptr, nullptr, hInst, nullptr
    );

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    // store instance handle in global variable
	g_hInst = hInstance;
	
    // initialize progress bar
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);
	
    // Process command line arguments
    int argc = 0;
    LPWSTR* wargv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // choose code page: CP_UTF8 for UTF-8, or CP_ACP for ANSI
    const UINT CODE_PAGE = CP_UTF8;

    // allocate array of char* (will own pointers to each C string)
    char** argv = (char**)malloc(sizeof(char*) * argc);
    if (!argv) 
    {
        LocalFree(wargv);
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    // convert each wide string to a newly allocated char* buffer
    for (int i = 0; i < argc; ++i) 
    {
        LPWSTR w = wargv[i];
        if (!w) 
        {
            argv[i] = nullptr;
            continue;
        }

        // get required size including null terminator
        int needed = WideCharToMultiByte(CODE_PAGE, 0, w, -1, nullptr, 0, nullptr, nullptr);
        if (needed <= 0) 
        {
            argv[i] = nullptr;
            continue;
        }

        char* buf = (char*)malloc(needed);
        if (!buf) 
        {
            // cleanup on partial failure
            for (int j = 0; j < i; ++j) free(argv[j]);
            free(argv);
            LocalFree(wargv);
            fprintf(stderr, "malloc failed for arg %d\n", i);
            return 1;
        }

        WideCharToMultiByte(CODE_PAGE, 0, w, -1, buf, needed, nullptr, nullptr);
        argv[i] = buf;
    }

    // keep original pointer so we can free later even if argv is advanced
    char** argv_start = argv;
    int argc_start = argc;
	
    // parse arguments
    HWND argH = NULL;
    int n;
    double sps = 44100;
    if (argc != 1)
    {
		// command line mode
        AttachToConsole(NEW_CONSOLE);
		// determine action
        if (strcmp(argv[1], "ew") == 0) { action = "wav"; }
        else if (strcmp(argv[1], "ewm") == 0) { action = "wav_mono"; }
        else if (strcmp(argv[1], "e") == 0) { action = "encode"; }
        else if (strcmp(argv[1], "d") == 0) { action = "decode"; }
        else if (strcmp(argv[1], "b") == 0) { action = "binary"; }
        else if (strcmp(argv[1], "he") == 0) { action = "hex"; }
        else if (strcmp(argv[1], "hd") == 0) { action = "hexdec"; }
        else if (strcmp(argv[1], "hb") == 0) { action = "hexbin"; }
        else if (strcmp(argv[1], "hbd") == 0) { action = "hexbindec"; }
        // check options
        n = get_options(argc, argv);
        argc -= n;
        argv += n;
        // generate morse code
        string arg_in;
        // choose max allowed chars based on requested action
        int max_chars = MAX_TXT_INPUT;
        if (action == "decode") max_chars = MAX_MORSE_INPUT;
        else if (action == "sound" || action == "wav" || action == "wav_mono") max_chars = MAX_SOUND_INPUT;

        // collect arguments but never exceed max_chars
        while (argc > 2 && static_cast<int>(arg_in.size()) < max_chars)
        {
            string part = arg_string(argv[2]);
            int remaining = max_chars - static_cast<int>(arg_in.size());
            if (remaining <= 0) break; // nothing more allowed
            if (static_cast<int>(part.size()) > remaining)
            {
                part = part.substr(0, remaining);
                cerr << "Maximum input size reached (" << max_chars << " characters).\n";
            }
            arg_in += part;
            argc -= 1;
            argv += 1;
        }

        if (action == "encode") { cout << m.morse_encode(arg_in) << "\n"; }
        else if (action == "binary") { cout << m.morse_binary(arg_in) << "\n"; }
        else if (action == "decode") { cout << m.morse_decode(arg_in) << "\n"; }
        else if (action == "hex") { cout << m.bin_morse_hexdecimal(arg_in, 0) << "\n"; }
        else if (action == "hexdec") { cout << m.hexdecimal_bin_txt(arg_in, 0) << "\n"; }
        else if (action == "hexbin") { cout << m.bin_morse_hexdecimal(arg_in, 1) << "\n"; }
        else if (action == "hexbindec") { cout << m.hexdecimal_bin_txt(arg_in, 1) << "\n"; }
        else if (action == "sound" || action == "wav" || action == "wav_mono")
        {
            string morse = m.morse_encode(arg_in);
            cout << arg_in << "\n";
            cout << morse << "\n";
            MakeMorseSafe(m);

            if (action == "wav")
            {
                MorseWav mw = MorseWav(morse.c_str(), m.frequency_in_hertz, m.words_per_minute, m.samples_per_second, 2);
            }
            else if (action == "wav_mono")
            {
                MorseWav mw = MorseWav(morse.c_str(), m.frequency_in_hertz, m.words_per_minute, m.samples_per_second, 1);
            }
        }
       //cout << "Press [Enter] key to close program . . .\n";
       int c = getchar();
       return 0;
    }
    else
    {
		// GUI mode
        ShowMorseApp(g_hWnd);
    }

    // when done, free all allocated buffers and arrays
    for (int i = 0; i < argc_start; ++i) 
    {
        free(argv_start[i]);   // safe even if argv was advanced
    }

    free(argv_start);
    LocalFree(wargv);
    return 0;
}
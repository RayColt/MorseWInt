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
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")

using namespace std;

// global collection of windows/threads
static HINSTANCE g_hInst = NULL;

// Utilities
void AttachToConsole()
{
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        FILE* fp = nullptr;

        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
    }
}

static void ParseArgs(int argc, wchar_t** argv, wchar_t& modeOut, HWND& hwndOut)
{
    modeOut = 0;
    hwndOut = NULL;
    if (argc <= 1) return;
    wstring a1 = argv[1];
    if (a1.size() >= 2 && (a1[0] == L'/' || a1[0] == L'-'))
    {
        wchar_t c = towlower(a1[1]);
        modeOut = c;
        size_t colon = a1.find(L':');
        if (colon != wstring::npos)
        {
            wstring num = a1.substr(colon + 1);
            if (!num.empty()) hwndOut = (HWND)_wcstoui64(num.c_str(), nullptr, 0);
        }
        else if (argc >= 3)
        {
            wstring a2 = argv[2];
            bool numeric = !a2.empty();
            for (wchar_t ch : a2) if (!iswdigit(ch)) { numeric = false; break; }
            if (numeric) hwndOut = (HWND)_wcstoui64(a2.c_str(), nullptr, 0);
        }
    }
}

static void SafeCloseHandle(HANDLE& h) 
{
    if (h) 
    { 
        CloseHandle(h);
        h = NULL; 
    }
}

// ---------------- MorseWInt GUI ----------------

enum {
	CID_ENCODE = 100, CID_DECODE = 101, CID_EDIT = 102, CID_MORSE = 103, CID_BIN = 104, CID_HEX = 105, CID_HEXBIN = 106, CID_M2WS = 107, CID_M2WM = 108, CID_WAVOUT = 109 };

// Create child controls on given window
static void CreateMorseControls(HWND hWnd)
{
    int radiobuttonX = 425;

    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    HWND hMorseLabel = CreateWindowExW(0, L"STATIC", L"MORSE or TXT:", 
        WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 120, 18, 
        hWnd, NULL, g_hInst, NULL);
    
    HWND hwAVLabel = CreateWindowExW(0, L"STATIC", L"WAV OUTPUT:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX, 190, 120, 18,
        hWnd, NULL, g_hInst, NULL);

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
    
    CreateWindowExW(
        0, L"BUTTON", L"Morse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 40, 165, 20,
        hWnd, (HMENU)CID_MORSE, g_hInst, NULL
    );

    CreateWindowExW(
        0, L"BUTTON", L"BinMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 65, 165, 20,
        hWnd, (HMENU)CID_BIN, g_hInst, NULL
    );

    CreateWindowExW(
        0, L"BUTTON", L"HexMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 90, 165, 20,
        hWnd, (HMENU)CID_HEX, g_hInst, NULL
    );

    CreateWindowExW(
        0, L"BUTTON", L"HexBinMorse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 115, 165, 20,
        hWnd, (HMENU)CID_HEXBIN, g_hInst, NULL
    );

    CreateWindowExW(
        0, L"BUTTON", L"Morse to Wav(s)",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 140, 165, 20,
        hWnd, (HMENU)CID_M2WS, g_hInst, NULL
    );

    CreateWindowExW(
        0, L"BUTTON", L"Morse to Wav(m)",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        radiobuttonX, 165, 165, 20,
        hWnd, (HMENU)CID_M2WM, g_hInst, NULL
    );

    HWND hWavOut = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE, radiobuttonX, 215, 165, 100, hWnd, (HMENU)CID_WAVOUT, g_hInst, NULL);
    HWND hProg = CreateWindowExW(
        0,
        PROGRESS_CLASSW,
        NULL,
        WS_CHILD | WS_VISIBLE,
        radiobuttonX, 400, 240, 20,
        hWnd,
        (HMENU)2001,
        g_hInst,
        NULL
    );
    SendMessageW(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, 100));   // 0–100%
    SendMessageW(hProg, PBM_SETPOS, 0, 0);                      // start at 0
	SendMessageW(hProg, PBM_SETPOS, 33, 0); // update position %
	//SendMessageW(hProg, PBM_STEPIT, 0, 0); // increment position by step amount
    //SendMessageW(hProg, PBM_SETBKCOLOR, 0, RGB(0, 0, 0));        // background
   // SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(255, 255, 255)); // bar color


    HWND hEncodeButton = CreateWindowExW(0, L"BUTTON", L"Encode", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 400, 80, 24, hWnd, (HMENU)CID_ENCODE, g_hInst, NULL);
    HWND hDecodeButton = CreateWindowExW(0, L"BUTTON", L"Decode", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 400, 80, 24, hWnd, (HMENU)CID_DECODE, g_hInst, NULL);

    SendMessageW(hMorseLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

   
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWavOut, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hEncodeButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hDecodeButton, WM_SETFONT, (WPARAM)hFont, TRUE);
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

static int ShowMorseApp()
{
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = MorseWIntWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"MorseWIntWindowClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
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
	g_hInst = hInstance;
	// Initialize common controls
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&icc);
    AttachToConsole();
    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    wchar_t mode = 0;
    HWND argH = NULL;
    ParseArgs(argc, argv, mode, argH);
    {
        char buf[256]{};
    }
    if (mode == 'p')
    {
        Morse m;
        string ss = m.morse_encode("sos ss");
        printf(ss.c_str());
        MessageBoxA(NULL, ss.c_str(), "Test", MB_OK);
    }
    if (mode == 'm')
    {
        ShowMorseApp();
    }
    if (mode == 'w')
    {

        ShowMorseApp();

        return 0;


    }

    LocalFree(argv);
    return 0;
}
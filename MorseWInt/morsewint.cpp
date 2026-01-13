/**
* MorseWInt.cpp : Defines the entry point for the application.
*/
#include <windows.h>
#include <random>
#include <string>
#include "morse.h"

#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")
using namespace std;

// Structs
struct ThreadParam
{
    RECT rc;
    // optional: pass as owner so 
    // overlay/preview behaviors remain
    HWND ownerForParent;
     const wchar_t* className;
};

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
// IDs
enum { CID_OK = 100, CID_CANCEL = 101, CID_EDIT_STARS = 110, CID_EDIT_SPEED = 111, CID_PREVIEW = 112, CID_BUTTON_COLOR = 113, CID_LABEL_COLOR = 114 };

// Create child controls on given window
static void CreateMorseControls(HWND dlg)
{
    wstring mst = L"Stars (max " + to_wstring(10) + L"):";
    wstring msp = L"Speed (max " + to_wstring(20) + L"):";
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    HWND hColorLabel = CreateWindowExW(0, L"STATIC", mst.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 10, 120, 18, dlg, NULL, g_hInst, NULL);
    HWND hColorEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT, 150, 8, 80, 20, dlg, (HMENU)CID_EDIT_STARS, g_hInst, NULL);
    HWND hSpeedLabel = CreateWindowExW(0, L"STATIC", msp.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 40, 120, 18, dlg, NULL, g_hInst, NULL);
    HWND hSpeedEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT, 150, 38, 80, 20, dlg, (HMENU)CID_EDIT_SPEED, g_hInst, NULL);
    HWND hColorButton = CreateWindowExW(0, L"BUTTON", L"Pick color", WS_CHILD | WS_VISIBLE, 250, 6, 80, 20, dlg, (HMENU)CID_BUTTON_COLOR, g_hInst, NULL);
    CreateWindowExW(0, L"STATIC", L"     ", WS_CHILD | WS_VISIBLE | SS_SIMPLE | SS_BLACKFRAME, 250, 38, 80, 20, dlg, (HMENU)CID_LABEL_COLOR, g_hInst, NULL);
    HWND hOkButton = CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 70, 80, 24, dlg, (HMENU)CID_OK, g_hInst, NULL);
    HWND hCancelButton = CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 168, 70, 80, 24, dlg, (HMENU)CID_CANCEL, g_hInst, NULL);

    SendMessageW(hColorLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hColorEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hSpeedLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hSpeedEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hColorButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hOkButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hCancelButton, WM_SETFONT, (WPARAM)hFont, TRUE);
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
            if (id == CID_OK)
            {
                BOOL ok;
                int stars = GetDlgItemInt(hWnd, CID_EDIT_STARS, &ok, FALSE);

                DestroyWindow(hWnd);
                return 0;
            }
            else if (id == CID_CANCEL)
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
    wc.lpszClassName = L"MorseWindowClass";

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
        if (argH)
        {
            LocalFree(argv);
            return 0;
        }
    }
    if (mode == 'm')
    {
		Morse m;
		string ss = m.morse_encode("sos ss");
        printf(ss.c_str());
        MessageBoxA(NULL, ss.c_str(), "Test", MB_OK);
        if (argH)
        {
			printf ("hello morse\n");
            

            return 0;
        }
    }
    if (mode == 'w')
    {

        ShowMorseApp();

        return 0;


    }

    LocalFree(argv);
    return 0;
}
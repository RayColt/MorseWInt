/**
* MorseWInt.cpp 
* 
* International Morse Code Encoder/Decoder/Wav creator with GUI and Command Line Interface in One!
* 
* @author RAY COLT
* @version 01111 010101 11111
*/
#include "main.h"

// ---------------- MorseWInt Helper Functions ----------------

/**
* Make morse settings safe
* 
* @param tone
* @param wpm
* @param sps
*/
static void MakeMorseSafe(double &tone, int &wpm, int &sps)
{
    if (sps < 8000.0) sps = 8000.0;
    if (sps > 48000) sps = 48000.0;
    if (tone < 20.0) tone = 20.0;
    if (tone > 8000.0) tone = 8000.0;
    if (wpm < 0.0) wpm = 0.0;
    if (wpm > 50.0) wpm = 50.0;
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
                frequency_in_hertz = atof(&argv[2][4]);
            }
            else if (strncmp(argv[2], "-wpm:", 5) == 0)
            {
                words_per_minute = atof(&argv[2][5]);
            }
            else if (strncmp(argv[2], "-sps:", 5) == 0)
            {
                samples_per_second = atof(&argv[2][5]);
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
* Parse int from edit field
* 
* @param hWnd
* @param defaultVal
* @return wstring
*/
static int ParseIntFromEdit(HWND hEdit, int defaultVal) 
{
    if (!hEdit) return defaultVal;
    wstring w = GetTextFromEditField(hEdit);
    if (w.empty()) return defaultVal;
    wchar_t* end = nullptr;
    errno = 0;
    long val = wcstol(w.c_str(), &end, 10);
    if (end == w.c_str() || errno == ERANGE) return defaultVal;
    return static_cast<int>(val);
}

/**
* Parse double from edit field
*
* @param hWnd
* @param defaultVal
* @return wstring
*/
static double ParseDoubleFromEdit(HWND hEdit, double defaultVal)
{
    if (!hEdit) return defaultVal;
    wstring w = GetTextFromEditField(hEdit);
    if (w.empty()) return defaultVal;
    wchar_t* end = nullptr;
    errno = 0;
    double val = wcstod(w.c_str(), &end);
    if (end == w.c_str() || errno == ERANGE) return defaultVal;
    return val;
}

/**
* Convert string to lowercase in place
* 
* @param s
*/
static void ToLowerInPlace(TCHAR* s)
{
    for (; *s; ++s) *s = (TCHAR)_totlower(*s);
}

/**
* Convert string to wstring
*
* @param str
* @return wstring
*/
wstring StringToWString(const string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

/**
* Convert wstring to string
*
* @param wstr
* @return string
*/
string WStringToString(const wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

/**
* Get text from edit field
*
* @param hWnd
* @return wstring
*/
wstring GetTextFromEditField(HWND hWnd)
{
    int len = (int)SendMessageW(hWnd, WM_GETTEXTLENGTH, 0, 0);
    wstring buf(len + 1, L'\0');
    SendMessageW(hWnd, WM_GETTEXT, (WPARAM)(len + 1), (LPARAM)buf.data());
    buf.resize(wcslen(buf.c_str()));
    return buf;
}

/**
* Trim decimals from string representation of float
*
* @param s
* @param decimals
* @return string
*/
string trimDecimals(const string& s, int decimals)
{
    int pos = s.find('.');
    if (pos == string::npos) return s;
    int end = pos + 1 + decimals;
    if (end >= s.size()) return s;
    return s.substr(0, end);
}

// ---------------- MorseWInt GUI ----------------

// Create child controls on given window
static void CreateMorseControls(HWND hWnd)
{
    // radiobutton x position
    int radiobuttonX = 425;
    int radiobuttonY = 30;
    int wavinY = 185;

    // Create ms font
    HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hFontMorse = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console");
    HFONT hFontBold = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hFontSmallBold = CreateFontW(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Create labels
    HWND hMorseLabel = CreateWindowExW(0, L"STATIC", L"MORSE / TXT:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, 10, 20, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    hCountLabel = CreateWindowExW(0, L"STATIC", StringToWString(to_string(MAX_TXT_INPUT)).c_str(),
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX - 190, radiobuttonY - 10, 25, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hModesLabel = CreateWindowExW(0, L"STATIC", L"MODES:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX, radiobuttonY - 20, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hWavInLabel = CreateWindowExW(0, L"STATIC", L"WAV INPUT:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX, wavinY, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hToneLabel = CreateWindowExW(0, L"STATIC", L"Tone",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX + 20, wavinY + 20, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hWpmLabel = CreateWindowExW(0, L"STATIC", L"Wpm:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX + 75, wavinY + 20, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hSpsLabel = CreateWindowExW(0, L"STATIC", L"Sps:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX + 145, wavinY + 20, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    HWND hWavOutLabel = CreateWindowExW(0, L"STATIC", L"WAV OUTPUT:",
        WS_CHILD | WS_VISIBLE | SS_LEFT, radiobuttonX, 255, 120, 18,
        hWnd, NULL, g_hInst, NULL);

    // Create edit box
    hEdit = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
        15, 40, 400, 300,
        hWnd, (HMENU)(INT_PTR)CID_EDIT, g_hInst, NULL);
    // subclass to handle Ctrl+A
    SetWindowSubclass(hEdit, Edit_SelectAll_SubclassProc, 1, 0);

    // Create wav output edit box
    hWavOut = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
        NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_MULTILINE | ES_READONLY,
        radiobuttonX, 275, 240, 120,
        hWnd, (HMENU)(INT_PTR)CID_WAVOUT, g_hInst, NULL);
    // subclass to handle Ctrl+A
    SetWindowSubclass(hWavOut, Edit_SelectAll_SubclassProc, 1, 0);

    // Create Tone edit box
    hTone = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
        NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT,
        radiobuttonX, wavinY + 40, 70, 18,
        hWnd, (HMENU)(INT_PTR)CID_TONE, g_hInst, NULL);

    // Create Wpm edit box
    hWpm = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
        NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT,
        radiobuttonX + 75, wavinY + 40, 65, 18,
        hWnd, (HMENU)(INT_PTR)CID_WPM, g_hInst, NULL);

    // Create Sps edit box
    hSps = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT",
        NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT,
        radiobuttonX + 145, wavinY + 40, 65, 18,
        hWnd, (HMENU)(INT_PTR)CID_SPS, g_hInst, NULL);

    // Create radio buttons
    HWND hMorse = CreateWindowExW(
        WS_EX_TRANSPARENT, L"BUTTON", L"Morse",
        WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP,
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

    // set default radio button selection
    SendMessageW(hMorse, BM_SETCHECK, BST_CHECKED, 0); // default selection

    // create progress bar
    hProg = CreateWindowExW(
        0,
        PROGRESS_CLASSW,
        NULL,
        WS_CHILD | WS_VISIBLE,
        268, 15, 146, 18,
        hWnd,
        (HMENU)CID_PROG,
        g_hInst,
        NULL
    );

    // Initialize and create progress bar
    INITCOMMONCONTROLSEX iccp = { sizeof(iccp), ICC_PROGRESS_CLASS };
    InitCommonControlsEx(&iccp);
    SendMessageW(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, 100));   // 0–100%
    SendMessageW(hProg, PBM_SETPOS, 0, 0);                      // start at 0
    SendMessageW(hEdit, EM_LIMITTEXT, (WPARAM)MAX_TXT_INPUT, 0); // limit text input

    // Create buttons
    HWND hEncodeButton = CreateWindowExW(0, L"BUTTON", L"ENCODE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 15, 355, 185, 40, hWnd, (HMENU)CID_ENCODE, g_hInst, NULL);
    HWND hDecodeButton = CreateWindowExW(0, L"BUTTON", L"DECODE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 225, 355, 185, 40, hWnd, (HMENU)CID_DECODE, g_hInst, NULL);

    // Set fonts
    SendMessageW(hMorseLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hModesLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hWavOutLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hWavInLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hToneLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hWpmLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hSpsLabel, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hCountLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Edit box
    SendMessageW(hEdit, WM_SETFONT, (WPARAM)hFontMorse, TRUE);
    SendMessageW(hWavOut, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Buttons
    SendMessageW(hEncodeButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
    SendMessageW(hDecodeButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);

    // Radio buttons
    SendMessageW(hMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBinMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hHexMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hHexBinMorse, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hMorseToWavS, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hMorseToWavM, WM_SETFONT, (WPARAM)hFont, TRUE);

    // set default morse settings in edit boxes
    wstring wt = StringToWString(trimDecimals(to_string(frequency_in_hertz), 3));
    wstring ww = StringToWString(to_string(words_per_minute));
    wstring ws = StringToWString(to_string(samples_per_second));
    SendMessageW(hTone, WM_SETTEXT, 0, (LPARAM)wt.c_str());
    SendMessageW(hWpm, WM_SETTEXT, 0, (LPARAM)ww.c_str());
    SendMessageW(hSps, WM_SETTEXT, 0, (LPARAM)ws.c_str());
}

// ---------------- MorseWInt Procedures ----------------

/**
* Show MorseWInt application window
* 
* @param hwnd
* @return int
*/
static int ShowMorseApp(HWND &hwnd)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = MorseWIntWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = L"MorseWIntWindowClass"; // TODO: add icon left corner???
    wc.hIcon = (HICON)LoadImage(NULL, L"app.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
   // wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClass(&wc);

    hwnd = CreateWindow(
        wc.lpszClassName,
        L"MorseWInt 01111 010101 11111",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        700, 460,
        nullptr, nullptr, g_hInst, nullptr
    );
    //HICON hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
    //SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

/**
* Wav thread procedure
*
* @param pv
*/
static unsigned __stdcall WavThreadProc(void* pv)
{
    WavThreadParams* p = static_cast<WavThreadParams*>(pv);
    if (!p) return 0;
    HWND hwnd = p->hwnd;
    // Prepare result allocated for the UI thread to delete
    WavThreadResult* res = new WavThreadResult();
    try
    {
        // Heavy work on background thread
        MorseWav mw(p->morse.c_str(), p->tone, p->wpm, p->sps, p->channels, p->showExternal);

        res->fullPath = StringToWString(mw.GetFullPath());
        FullPath = mw.GetFullPath();
        res->tone = p->tone;
        res->wpm = static_cast<int>(p->wpm);
        res->sps = static_cast<int>(p->sps);
        res->waveSize = mw.GetWaveSize();
        res->pcmCount = mw.GetPcmCount();
        res->channels = p->channels;
    }
    catch (const exception& e)
    {
        res->fullPath = StringToWString(string("ERROR: ") + e.what());
        res->tone = p->tone;
        res->wpm = static_cast<int>(p->wpm);
        res->sps = static_cast<int>(p->sps);
        res->waveSize = 0;
        res->pcmCount = 0;
        res->channels = p->channels;
    }
    catch (...)
    {
        res->fullPath = StringToWString(string("ERROR: unknown exception"));
        res->tone = p->tone;
        res->wpm = static_cast<int>(p->wpm);
        res->sps = static_cast<int>(p->sps);
        res->waveSize = 0;
        res->pcmCount = 0;
        res->channels = p->channels;
    }
    PostMessageW(hwnd, WM_MWAV_DONE, reinterpret_cast<WPARAM>(res), 0);
    delete p;
    return 0;
}

/**
* Console Wav thread procedure
*
* @param pv
*/
static unsigned __stdcall ConsoleWavThreadProc(void* pv)
{
    ConsoleWavParams* p = static_cast<ConsoleWavParams*>(pv);
    if (!p) return 0;
    try
    {
        MorseWav mw(p->morse.c_str(), p->tone, p->wpm, p->sps, p->channels, p->showExternal);
    }
    catch (const exception& e)
    {
        cerr << "ERROR creating WAV: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "ERROR creating WAV: unknown exception" << endl;
    }
    delete p;
    return 0;
}

/**
* Subclass procedure for edit controls: handle Ctrl+A -> select all
*
* @param hwnd
* @param uMsg
* @param wParam
* @param lParam
* @param uIdSubclass
* @param dwRefData
* @return LRESULT
*/
static LRESULT CALLBACK Edit_SelectAll_SubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_KEYDOWN)
    {
        // check Ctrl + A (handle both 'A' and 'a')
        if ((wParam == 'A' || wParam == 'a') && (GetKeyState(VK_CONTROL) & 0x8000))
        {
            SendMessageW(hwnd, EM_SETSEL, 0, -1); // select all text
            return 0; // consumed
        }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

/**
* Window Procedure for MorseWInt
*
* @param hWnd
* @param msg
* @param wParam
* @param lParam
*
* @return LRESULT
*/
static LRESULT CALLBACK MorseWIntWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        CreateMorseControls(hWnd);
        wstring out = L"See command line for help:\r\n morse.exe -h or -help";
        SendMessageW(hWavOut, WM_SETTEXT, 0, (LPARAM)out.c_str());
        return 0;
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        int code = HIWORD(wParam);

        if (id == CID_EDIT && code == EN_CHANGE)
        {
            int len = GetWindowTextLengthW(hEdit); // number of characters
            int left = MAX_TXT_INPUT - len;
            wstring out = StringToWString(to_string(left));
            SendMessageW(hCountLabel, WM_SETTEXT, 0, (LPARAM)out.c_str());
            if (len == 0)
            {
                SendMessageW(hProg, PBM_SETPOS, 0, 0); // update position %
                SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(0, 144, 255)); // bar color
            }
            if (len > 0 && len < MAX_TXT_INPUT)
            {
                int percent = (len * 100) / MAX_TXT_INPUT;
                if (percent > 100) percent = 100;
                SendMessageW(hProg, PBM_SETPOS, percent, 0); // update position % 
                if (percent < 90)
                    SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(0, 144, 255));
                if (percent >= 95)
                    SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(255, 66, 0));
                if (percent >= 99)
                    SendMessageW(hProg, PBM_SETBARCOLOR, 0, RGB(255, 0, 0));
            }
        }

        bool b1 = false, b2 = false, b3 = false, b4 = false, b5 = false, b6 = false, b7 = false;
        if (IsDlgButtonChecked(hWnd, CID_MORSE) == BST_CHECKED) { b1 = true; }
        else if (IsDlgButtonChecked(hWnd, CID_BIN) == BST_CHECKED) { b2 = true; }
        else if (IsDlgButtonChecked(hWnd, CID_HEX) == BST_CHECKED) { b3 = true; }
        else if (IsDlgButtonChecked(hWnd, CID_HEXBIN) == BST_CHECKED) { b4 = true; }
        else if (IsDlgButtonChecked(hWnd, CID_M2WS) == BST_CHECKED) { b5 = true; }
        else if (IsDlgButtonChecked(hWnd, CID_M2WM) == BST_CHECKED) { b6 = true; }

        wstring in = GetTextFromEditField(hEdit);
        string tmp;
        wstring out;

        if (id == CID_ENCODE && code == BN_CLICKED)
        {
            if (b1)
            {
                tmp = m.morse_encode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b2)
            {
                tmp = m.morse_binary(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b3)
            {
                tmp = m.bin_morse_hexdecimal(WStringToString(in), 0);
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b4)
            {
                tmp = m.bin_morse_hexdecimal(WStringToString(in), 1);
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b5)
            {
                tmp = m.morse_encode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());

                int si = ParseIntFromEdit(hSps, samples_per_second);
                double ti = ParseDoubleFromEdit(hTone, frequency_in_hertz);
                int wi = ParseIntFromEdit(hWpm, words_per_minute);
                MakeMorseSafe(ti, wi, si);
                wstring  tonein = StringToWString(trimDecimals(to_string(ti), 3));
                wstring wpmin = StringToWString(to_string(wi));
                wstring spsin = StringToWString(to_string(si));

                // show a short "generating" message
                wstring generating = L"Generating WAV file, please wait...";
                SendMessageW(hWavOut, WM_SETTEXT, 0, (LPARAM)generating.c_str());

                // disable encode/decode while background work runs
                EnableWindow(GetDlgItem(hWnd, CID_ENCODE), FALSE);
                EnableWindow(GetDlgItem(hWnd, CID_DECODE), FALSE);

                // allocate thread params
                WavThreadParams* p = new WavThreadParams();
                p->morse = tmp; // tmp is the morse string already computed
                p->tone = stod(WStringToString(tonein));
                p->wpm = stod(WStringToString(wpmin));
                p->sps = stod(WStringToString(spsin));
                p->channels = STEREO;
                p->hwnd = hWnd;
                p->showExternal = SHOW_EXTERNAL_MEDIAPLAYER;

                // start thread (CRT-friendly)
                uintptr_t h = _beginthreadex(NULL, 0, &WavThreadProc, p, 0, NULL);
                if (h != 0) CloseHandle(reinterpret_cast<HANDLE>(h));
            }
            else if (b6)
            {
                tmp = m.morse_encode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());

                int si = ParseIntFromEdit(hSps, samples_per_second);
                double ti = ParseDoubleFromEdit(hTone, frequency_in_hertz);
                int wi = ParseIntFromEdit(hWpm, words_per_minute);
                MakeMorseSafe(ti, wi, si);
                wstring  tonein = StringToWString(trimDecimals(to_string(ti), 3));
                wstring wpmin = StringToWString(to_string(wi));
                wstring spsin = StringToWString(to_string(si));

                // show a short "generating" message
                wstring generating = L"Generating WAV file, please wait...";
                SendMessageW(hWavOut, WM_SETTEXT, 0, (LPARAM)generating.c_str());

                // disable encode/decode while background work runs
                EnableWindow(GetDlgItem(hWnd, CID_ENCODE), FALSE);
                EnableWindow(GetDlgItem(hWnd, CID_DECODE), FALSE);

                // allocate thread params
                WavThreadParams* p = new WavThreadParams();
                p->morse = tmp; // tmp is the morse string already computed
                p->tone = stod(WStringToString(tonein));
                p->wpm = stod(WStringToString(wpmin));
                p->sps = stod(WStringToString(spsin));
                p->channels = MONO;
                p->hwnd = hWnd;
				p->showExternal = SHOW_EXTERNAL_MEDIAPLAYER;

                // start thread (CRT-friendly)
                uintptr_t h = _beginthreadex(NULL, 0, &WavThreadProc, p, 0, NULL);
                if (h != 0) CloseHandle(reinterpret_cast<HANDLE>(h));
            }
            return 0;
        }
        else if (id == CID_DECODE && code == BN_CLICKED)
        {
            if (b1)
            {
                tmp = m.morse_decode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b2)
            {
                tmp = m.morse_decode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b3)
            {
                tmp = m.hexdecimal_bin_txt(WStringToString(in), 0);
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            else if (b4)
            {
                tmp = m.hexdecimal_bin_txt(WStringToString(in), 1);
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            if (b5)
            {
                tmp = m.morse_decode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            if (b6)
            {
                tmp = m.morse_decode(WStringToString(in));
                out = StringToWString(tmp);
                SendMessageW(hEdit, WM_SETTEXT, 0, (LPARAM)out.c_str());
            }
            return 0;
        }
        break;
    }
    case WM_MWAV_DONE:
    {
        EnableWindow(GetDlgItem(hWnd, CID_PAUSE), TRUE);
        WavThreadResult* res = reinterpret_cast<WavThreadResult*>(wParam);
        if (res)
        {
            // Compose same output the UI used to display
            wstring spsin = StringToWString(to_string(res->sps));
            wstring tonein = StringToWString(trimDecimals(to_string(res->tone), 3));
            wstring wpmin = StringToWString(to_string(res->wpm));
            wstring wout = res->fullPath + L" (" + StringToWString(trimDecimals(to_string(res->waveSize / 1024.0), 2)) + L" kB)\r\n\r\n";
            wout += L"wave: " + spsin + L" Hz (-sps:" + spsin + L")\r\n";
            wout += L"tone: " + tonein + L" Hz (-hz:" + tonein + L")\r\n";
            wout += L"code: " + wpmin + L" Hz (-wpm:" + wpmin + L")\r\n";
            wout += StringToWString(to_string(res->pcmCount * res->channels)) + L" PCM samples in ";
            wout += StringToWString(trimDecimals(to_string(res->pcmCount / stod(spsin)), 2)) + L" s\r\n";

            SendMessageW(hWavOut, WM_SETTEXT, 0, (LPARAM)wout.c_str());
            SendMessageW(hTone, WM_SETTEXT, 0, (LPARAM)tonein.c_str());
            SendMessageW(hWpm, WM_SETTEXT, 0, (LPARAM)wpmin.c_str());
            SendMessageW(hSps, WM_SETTEXT, 0, (LPARAM)spsin.c_str());

            // Re-enable buttons that were disabled while creating the wav
            EnableWindow(GetDlgItem(hWnd, CID_ENCODE), TRUE);
            EnableWindow(GetDlgItem(hWnd, CID_DECODE), TRUE);

            delete res;
        }
        return 0;
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

/**
* Creates new output console
*
*/
static void AttachToNewConsole()
{
    AllocConsole();
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
}

/**
* Check if a console is available (e.g., launched from cmd)
* 
* @return bool
*/
static bool HasConsole()
{
    HWND hwndConsole = GetConsoleWindow();
    if (hwndConsole != NULL) return true;

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hIn != INVALID_HANDLE_VALUE && hIn != NULL)
    {
        DWORD mode;
        if (GetConsoleMode(hIn, &mode))
            return true; // real console input handle
    }

    //Fallback: check CRT stdin file descriptor is valid before calling _isatty
    int fh = _fileno(stdin);
    if (fh < 0) return false; // IMPORTANT: avoid calling _isatty if fh < 0
    return _isatty(fh) != 0;
}

/**
* Windows application entry point
* 
* @param hInstance
*/
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    // store instance handle in global variable
	g_hInst = hInstance;

	CheckRadioButton(g_hWnd, CID_MORSE, CID_M2WM, CID_MORSE); // default selection
	
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
        
        // command line mode
        AttachToNewConsole();
        if (!HasConsole())
        {
            // No console available: fallback UI
            MessageBoxW(NULL, L"No console available and no command-line arguments provided. When Using WIN+R.", L"Morse", MB_OK | MB_ICONINFORMATION);
            return 0;
        }

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
			MakeMorseSafe(frequency_in_hertz, words_per_minute, samples_per_second);
            if (action == "wav")
            {
                // start background thread to create stereo wav
                ConsoleWavParams* p = new ConsoleWavParams();
                p->morse = morse;
                p->tone = frequency_in_hertz;
                p->wpm = words_per_minute;
                p->sps = samples_per_second;
                p->channels = STEREO;
				p->showExternal = SHOW_EXTERNAL_MEDIAPLAYER;

                uintptr_t th = _beginthreadex(NULL, 0, &ConsoleWavThreadProc, p, 0, NULL);
                if (th != 0)
                {
                    // wait for completion (you may remove waiting to fire-and-forget)
                    WaitForSingleObject(reinterpret_cast<HANDLE>(th), INFINITE);
                    CloseHandle(reinterpret_cast<HANDLE>(th));
                }
                else
                {
                    // fallback to synchronous if thread creation failed
                    delete p;
                    try { MorseWav mw = MorseWav(morse.c_str(), frequency_in_hertz, words_per_minute, samples_per_second, STEREO, SHOW_EXTERNAL_MEDIAPLAYER); }
                    catch (...) { cerr << "Failed to create WAV (fallback)." << endl; }
                }
            }
            else if (action == "wav_mono")
            {
                // start background thread to create mono wav
                ConsoleWavParams* p = new ConsoleWavParams();
                p->morse = morse;
                p->tone = frequency_in_hertz;
                p->wpm = words_per_minute;
                p->sps = samples_per_second;
                p->channels = MONO;
                p->showExternal = SHOW_EXTERNAL_MEDIAPLAYER;

                uintptr_t th = _beginthreadex(NULL, 0, &ConsoleWavThreadProc, p, 0, NULL);
                if (th != 0)
                {
                    WaitForSingleObject(reinterpret_cast<HANDLE>(th), INFINITE);
                    CloseHandle(reinterpret_cast<HANDLE>(th));
                }
                else
                {
                    delete p;
                    try { MorseWav mw = MorseWav(morse.c_str(), frequency_in_hertz, words_per_minute, samples_per_second, MONO, SHOW_EXTERNAL_MEDIAPLAYER); }
                    catch (...) { cerr << "Failed to create WAV (fallback)." << endl; }
                }
            }
        }
        cout << "\nPress [Enter] key to close program . . .\n";
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
        free(argv_start[i]);
    }

    free(argv_start);
    LocalFree(wargv);
    return 0;
}

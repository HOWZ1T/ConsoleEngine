#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <dshow.h>
#include <chrono>
#include <thread>
#include <wincon.h>
#include <atomic>
#include <condition_variable>
#include <vector>

#include "Colors.h"
#include "Chars.h"
#include "Rect.h"

static WCHAR DEFAULT_CHAR = PIXEL_SOLID;
static WORD DEFAULT_ATTRIBS = FG_WHITE | BG_BLACK;

class Console {
private:
    void GameThread();

protected:
    struct KeyState {
        bool pressed;
        bool released;
        bool held;
    } keys[256], mouse[5];

    short keyOldState[256] = { 0 };
    short keyNewState[256] = { 0 };
    bool mouseOldState[5] = { 0 };
    bool mouseNewState[5] = { 0 };
    bool consoleInFocus = true;

    HANDLE hConsole, hConsoleIn;
    CHAR_INFO* scrBuffer;
    SMALL_RECT rectWindow;
    CONSOLE_CURSOR_INFO cursorInfo;

    int scrWidth, scrHeight, fontWidth, fontHeight, mousePosX, mousePosY;
    const wchar_t* title;

    static std::mutex mux;
    static std::condition_variable gameFinished;
    static std::atomic<bool> atomActive;

    void WError(wchar_t function);
    void Error(LPTSTR msg);
    static BOOL __RPC_CALLEE CloseHandler(DWORD evt);

public:
    ~Console();

    void CreateConsole(int width, int height, int fontWidth = 12, int fontHeight = 12, wchar_t title[] = L"Console");
    void Start();

    KeyState GetKey(int keyID);
    KeyState GetMouse(int mouseBtnID);

    int MouseX();
    int MouseY();
    bool IsFocused();

    int ScreenWidth();
    int ScreenHeight();

    // -- USER SHOULD OVERRIDE THESE --
    virtual bool OnCreate() = 0;
    virtual bool OnUpdate(float elapsedTime) = 0;
    virtual bool OnDraw() = 0;

    // -- OPTIONAL: for resource clean up --
    virtual bool OnDestory() {return true;}

    // -- UTILITIES --
    std::vector<POINT> BrensenhamLine(int x1, int y1, int x2, int y2);
    std::vector<POINT> MidPointOval(int rx, int ry, int xc, int yc);

    // -- DRAWING ROUTINES --
    bool Clear(WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Draw(int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Rect(struct Rect rect, bool fill = false, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS,
            WORD fillChr = PIXEL_SOLID, WORD fillAttributes = DEFAULT_ATTRIBS);

    bool Line(int x, int y, int x1, int y1, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Oval(int rx, int ry, int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Circle(int r, int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);

    bool DrawTextW(int x, int y, wchar_t text[], WORD attributes = DEFAULT_ATTRIBS);
};
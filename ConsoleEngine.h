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
    HANDLE hConsole, hConsoleIn;
    CHAR_INFO* scrBuffer;
    SMALL_RECT rectWindow;

    int scrWidth, scrHeight, fontWidth, fontHeight;
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
    bool Draw(int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Rect(struct Rect rect, bool fill = false, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS,
            WORD fillChr = PIXEL_SOLID, WORD fillAttributes = DEFAULT_ATTRIBS);

    bool Line(int x, int y, int x1, int y1, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Oval(int rx, int ry, int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
    bool Circle(int r, int x, int y, WCHAR chr = DEFAULT_CHAR, WORD attributes = DEFAULT_ATTRIBS);
};
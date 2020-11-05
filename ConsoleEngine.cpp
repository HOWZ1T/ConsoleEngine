#include "ConsoleEngine.h"

std::atomic<bool> Console::atomActive(false);
std::condition_variable Console::gameFinished;
std::mutex Console::mux;


void Console::CreateConsole(int width, int height, int fontWidth, int fontHeight, wchar_t title[]) {
    scrWidth = width;
    scrHeight = height;
    this->fontWidth = fontWidth;
    this->fontHeight = fontHeight;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        WError((wchar_t) L"GetStdHandle");
    }

    hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hConsoleIn == INVALID_HANDLE_VALUE) {
        WError((wchar_t) L"GetStdHandle");
    }

    this->title = title;

    if(!SetConsoleTitle(title)) {
        WError((wchar_t) L"SetConsoleTitle");
    }

    // change console visual size to a minimum so ScreenBuffer can shrink below visual size
    rectWindow = {0, 0, 1, 1};
    if(!SetConsoleWindowInfo(hConsole, true, &rectWindow)) {
        WError((wchar_t) L"SetConsoleWindowInfo");
    }

    // set size of screen buffer
    COORD coord = {(short) scrWidth, (short) scrHeight};
    if(!SetConsoleScreenBufferSize(hConsole, coord)) {
        WError((wchar_t) L"SetConsoleScreenBufferSize");
    }

    // assign screen buffer to the console
    if(!SetConsoleActiveScreenBuffer(hConsole)) {
        WError((wchar_t) L"SetConsoleActiveScreenBuffer");
    }

    // set font size
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = fontWidth;
    cfi.dwFontSize.Y = fontHeight;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy_s(cfi.FaceName, L"Consolas");
    if(!SetCurrentConsoleFontEx(hConsole, false, &cfi)) {
        WError((wchar_t) L"SetCurrentConsoleFontEx");
    }

    // get screen buffer info and check maximum allowed window size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        WError((wchar_t) L"GetConsoleScreenBufferInfo");
    }

    if (scrWidth > csbi.dwMaximumWindowSize.X) {
        Error(L"Screen Width / Font Width is Too Big");
    }

    if (scrHeight > csbi.dwMaximumWindowSize.Y) {
        Error(L"Screen Height / Font Height is Too Big");
    }

    // set physical console window size
    rectWindow = {0, 0, (short)(scrWidth-1), (short)(scrHeight-1)};
    if (!SetConsoleWindowInfo(hConsole, true, &rectWindow)) {
        WError((wchar_t) L"SetConsoleWindowInfo");
    }

    // set flags to allow mouse input
    if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
        WError((wchar_t) L"SetConsoleMode");
    }

    // allocate memory for screen buffer
    scrBuffer = new CHAR_INFO[scrWidth * scrHeight];
    memset(scrBuffer, 0, sizeof(CHAR_INFO) * scrWidth * scrHeight);

    // set close handler
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);

    // call user create function
    if(!OnCreate()) {
        Error( L"OnCreate");
    }
}

Console::~Console() {
    delete[] scrBuffer;
}

void Console::WError(wchar_t function) {
    // retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL
    );

    // display the error message and exit
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)function) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    function, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

void Console::Error(LPTSTR msg) {
    LPVOID lpDisplayBuf;
    // display the error message and exit
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)msg) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s"),
                    msg);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpDisplayBuf);
    ExitProcess(-1);
}

BOOL Console::CloseHandler(DWORD evt) {
    if (evt == CTRL_CLOSE_EVENT || evt == CTRL_BREAK_EVENT || evt == CTRL_SHUTDOWN_EVENT || evt == CTRL_LOGOFF_EVENT)
    {
        atomActive = false;

        // wait for thread to be exited
        std::unique_lock<std::mutex> ul(mux);
        gameFinished.wait(ul);
    }

    return true;
}

void Console::Start() {
    atomActive = true;
    std::thread t = std::thread(&Console::GameThread, this);

    // wait for thread to close
    t.join();
}

void Console::GameThread() {
    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    while (atomActive) {
        while (atomActive) {
            // handle timing
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTimeDuration = tp2 - tp1;
            tp1 = tp2;

            float elapsedTime = elapsedTimeDuration.count();

            // call user update function
            atomActive = OnUpdate(elapsedTime);

            // call user draw function
            if(!OnDraw()) {
                WError((wchar_t) L"OnDraw");
            }

            // update title and present screen buffer
            wchar_t s[256];
            swprintf_s(s, L"%s - FPS: %3.2f", title, 1.0f / elapsedTime);
            SetConsoleTitle(s);
            WriteConsoleOutput(hConsole, scrBuffer, { (short)scrWidth, (short)scrHeight }, { 0,0 }, &rectWindow);
        }

        if (OnDestory()) {
            delete[] scrBuffer;
            gameFinished.notify_one();
        } else {
            // destroy was cancelled by user for some reason, therefore continue running
            atomActive = true;
        }
    }
}

int Console::ScreenWidth() {
    return scrWidth;
}

int Console::ScreenHeight() {
    return scrHeight;
}

std::vector<POINT> Console::BrensenhamLine(int x1, int y1, int x2, int y2) {
    std::vector<POINT> points;
    int m = 2 * (y2 - y1);
    int slopeError = m - (x2 - x1);

    for(int x = x1, y = y1; x <= x2; x++) {
        points.push_back({x, y});

        // add slope to increment angle formed
        slopeError += m;

        // slope error reached limit, time to increment y and update slope error
        if (slopeError >= 0) {
            y++;
            slopeError -= 2 * (x2 - x1);
        }
    }

    return points;
}

std::vector<POINT> Console::MidPointOval(int rx, int ry, int xc, int yc) {
    std::vector<POINT> points;

    float dx, dy, d1, d2;
    int x = 0, y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry) - (rx * rx * ry) +
         (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy)
    {

        // Print points based on 4-way symmetry
        points.push_back({(int)(x+xc), (int)(y+yc)});
        points.push_back({(int)(-x+xc), (int)(y+yc)});
        points.push_back({(int)(x+xc), (int)(-y+yc)});
        points.push_back({(int)(-x+xc), (int)(-y+yc)});

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else
        {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
         ((rx * rx) * ((y - 1) * (y - 1))) -
         (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0)
    {

        // Print points based on 4-way symmetry
        points.push_back({(int)(x+xc), (int)(y+yc)});
        points.push_back({(int)(-x+xc), (int)(y+yc)});
        points.push_back({(int)(x+xc), (int)(-y+yc)});
        points.push_back({(int)(-x+xc), (int)(-y+yc)});

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else
        {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }

    return points;
}

bool Console::Draw(int x, int y, WCHAR chr, WORD attributes) {
    if (x < 0 || x >= scrWidth || y < 0 || y >= scrHeight) return false;

    scrBuffer[y * scrWidth + x].Char.UnicodeChar = chr;
    scrBuffer[y * scrWidth + x].Attributes = attributes;
    return true;
}

bool

Console::Rect(struct Rect rect, bool fill, WCHAR chr, WORD attributes, WORD fillChr, WORD fillAttributes) {
    if (!rect.inBounds(rectWindow)) {
        return false;
    }

    // draw border
    for (short x = rect.x; x <= rect.x1(); x++) {
        Draw(x, rect.y, chr, attributes);
        Draw(x, rect.y1(), chr, attributes);
    }

    for (short y = rect.y; y <= rect.y1(); y++) {
        Draw(rect.x, y, chr, attributes);
        Draw(rect.x1(), y, chr, attributes);
    }

    if (fill) {
        for (short x = rect.x+1; x <= rect.x1()-1; x++) {
            for (short y = rect.y+1; y <= rect.y1()-1; y++) {
                Draw(x, y, fillChr, fillAttributes);
            }
        }
    }

    return true;
}

bool Console::Line(int x1, int y1, int x2, int y2, WCHAR chr, WORD attributes) {
    if (x1 < 0 || x1 >= scrWidth || y1 < 0 || y1 >= scrHeight || x2 < 0 || x2 >= scrWidth || y2 < 0 || y2 >= scrHeight) {
        return false;
    }

    for(POINT p : BrensenhamLine(x1, y1, x2, y2)) {
        Draw(p.x, p.y, chr, attributes);
    }

    return true;
}

bool Console::Oval(int rx, int ry, int x, int y, WCHAR chr, WORD attributes) {
    int x1 = x - rx;
    int y1 = y - ry;
    int x2 = x + rx;
    int y2 = y + ry;

    if (x1 < 0 || x1 >= scrWidth || y1 < 0 || y1 >= scrHeight || x2 < 0 || x2 >= scrWidth || y2 < 0 || y2 >= scrHeight) {
        return false;
    }

    for(POINT p : MidPointOval(rx, ry, x, y)) {
        Draw(p.x, p.y, chr, attributes);
    }

    return true;
}

bool Console::Circle(int r, int x, int y, WCHAR chr, WORD attributes) {
    int x1 = x - r;
    int y1 = y - r;
    int x2 = x + r;
    int y2 = y + r;

    if (x1 < 0 || x1 >= scrWidth || y1 < 0 || y1 >= scrHeight || x2 < 0 || x2 >= scrWidth || y2 < 0 || y2 >= scrHeight) {
        return false;
    }

    for(POINT p : MidPointOval(r, r, x, y)) {
        Draw(p.x, p.y, chr, attributes);
    }

    return true;
}

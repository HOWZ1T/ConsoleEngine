#include "ConsoleEngine.h"
#include "Rect.h"
#include <string>

class TestConsole : public Console {
    int lastReleased = -1;

    bool OnCreate() override {
        return true;
    }

    bool OnUpdate(float elapsedTime) override {
        return true;
    }

    bool OnDraw() override {
        Clear(U_NONE, BG_BLACK);

        for(int x = 0; x < scrWidth; x++) {
            Draw(x, 0, std::to_wstring(x%10).at(0), FG_RED | BG_BLACK);
        }

        for(int y = 0; y < scrHeight; y++) {
            Draw(0, y, std::to_wstring(y%10).at(0), FG_RED | BG_BLACK);
        }

        wchar_t buf[256];
        swprintf(buf, 256, L"Mouse: (%d, %d)\0", mousePosX, mousePosY);
        DrawTextW(2, 2, buf);

        swprintf(buf, 256, L"InFocus: %s\0", consoleInFocus ? L"true" : L"false");
        DrawTextW(2, 3, buf);

        // get last key released
        for (int i = 0; i < 256; i++) {
            if (keys[i].released){
                lastReleased = i;
            }
        }

        swprintf(buf, 256, L"KeyReleased: %d\0", lastReleased);
        DrawTextW(2, 4, buf);

        return true;
    }
};

int main() {
    TestConsole con = TestConsole();
    con.CreateConsole(80, 45, 16, 16);
    con.Start();
    return 0;
}
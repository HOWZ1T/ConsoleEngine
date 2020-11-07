#include "ConsoleEngine.h"

class TestConsole : public Console {
    int lastReleased = -1;
    Sprite* sprite = new Sprite(0, 0, nullptr);

    bool OnCreate() override {
        Sprite::FromFile(sprite, L"./sprites/brick.con");
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

        DrawSprite(sprite, 10, 10);
        DrawTextW(2, 6, (wchar_t*) std::to_wstring(sprite->Width()).c_str());
        DrawTextW(2, 7, (wchar_t*) std::to_wstring(sprite->Height()).c_str());
        Draw(mousePosX, mousePosY, PIXEL_SOLID, FG_CYAN);

        return true;
    }

    bool OnDestory() override {
        delete sprite;
        return true;
    }
};

int main() {
    TestConsole con = TestConsole();
    con.CreateConsole(80, 45, 16, 16);
    con.Start();
    return 0;
}
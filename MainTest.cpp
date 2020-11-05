#include "ConsoleEngine.h"
#include "Rect.h"
#include <string>

class TestConsole : public Console {
    bool OnCreate() override {
        return true;
    }

    bool OnUpdate(float elapsedTime) override {
        return true;
    }

    bool OnDraw() override {
        for(int x = 0; x < scrWidth; x++) {
            Draw(x, 0, std::to_wstring(x%10).at(0), FG_RED | BG_BLACK);
        }

        for(int y = 0; y < scrHeight; y++) {
            Draw(0, y, std::to_wstring(y%10).at(0), FG_RED | BG_BLACK);
        }

        Line(1, 2, 9, 4, U_ARROW_RIGHT);
        Oval(3, 5, 10, 10);
        return true;
    }
};

int main() {
    TestConsole con = TestConsole();
    con.CreateConsole(80, 45, 16, 16);
    con.Start();
    return 0;
}
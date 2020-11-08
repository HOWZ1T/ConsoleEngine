#ifndef CONSOLE_ENGINE_SPRITE_H
#define CONSOLE_ENGINE_SPRITE_H

#ifndef UNICODE
#define UNICODE
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

class Sprite {
private:
    int width, height;
    short* buffer;

public:
    Sprite(int width, int height, short buffer[]);
    Sprite();
    ~Sprite();

    int Width();
    int Height();

    static bool FromFile(Sprite* sprite, wchar_t fp[]);
    short Sample(float x, float y);
    short GetPixel(int x, int y);
};

#endif
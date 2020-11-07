#include "Sprite.h"

Sprite::Sprite(int width, int height, short buffer[]) {
    this->width = width;
    this->height = height;
    this->buffer = buffer;
}

int Sprite::Width() {
    return width;
}

int Sprite::Height() {
    return height;
}

bool Sprite::FromFile(Sprite* sprite, wchar_t fp[]) {
    int i = 0, w = 0, h = 0;
    short *buff = nullptr;

    std::ifstream infile(fp);
    if (!infile.is_open()) {
        std::cerr << "unable to open file" << std::endl;
        exit(1);
    }

    std::string line;
    bool header = true;
    while(std::getline(infile, line)) {
        std::stringstream ss(line);
        std::string a, b, c;

        std::getline(ss, a, ',');
        std::getline(ss, b, ',');
        std::getline(ss, c, ',');

        if(header) {
            w = std::stoi(a);
            h = std::stoi(b);
            buff = new short[w*h];
            header = false;
        } else {
            buff[std::stoi(b) * w + std::stoi(a)] = std::stoi(c);
        }
    }
    infile.close();

    sprite->width = w;
    sprite->height = h;
    sprite->buffer = buff;

    return true;
}

Sprite::~Sprite() {
    delete[] buffer;
}

short Sprite::Sample(float x, float y) {
    int sx = ((float)x * width);
    int sy = ((float)y * height);
    return buffer[sy * width + sx];
}

short Sprite::GetPixel(int x, int y) {
    if (y * width + x < 0 || y * width + x >= width * height) return 0;
    return buffer[y * width + x];
}

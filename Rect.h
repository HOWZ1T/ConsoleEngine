#pragma once

struct Rect {
public:
    SHORT x, y, w, h;

    SHORT x1() {
        return x + w;
    }

    SHORT y1() {
        return y + h;
    }

    bool overlap(Rect other) {
        return !((x1() <= other.x || x >= other.x1()) || (y1() <= other.y | y >= other.y1()));
    }

    bool inBounds(Rect other) {
        return x >= other.x && x <= other.x1() && y >= other.y && y <= other.y1();
    }

    bool inBounds(SMALL_RECT other) {
        return x >= other.Left && x <= other.Right && y >= other.Top && y <= other.Bottom;
    }

    SMALL_RECT toSmallRect() {
        return SMALL_RECT {
            x, y, x1(), y1()
        };
    }

    static Rect fromSmallRect(SMALL_RECT sr) {
        return Rect{sr.Left, sr.Top, (SHORT) (sr.Right - sr.Left), (SHORT) (sr.Top - sr.Bottom)};
    }
};

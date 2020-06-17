#pragma once
#include "Console.h"
#include "TextVideo.h"
#include "BufferObject.hpp"
class ConsoleEngine
{
private:
    short width;
    short height;
    BufferObject buffer;
    Console* ptr;
    bool wcharMode;
public:
    ConsoleEngine(Console& window, bool wcharMode=false) :width(window.getWidth()), height(window.getHeight()), buffer(height, width), ptr(&window), wcharMode(wcharMode) 
    {
        CHAR_INFO default;
        default.Attributes = 7;
        if (wcharMode)
            default.Char.AsciiChar = ' ';
        else
            default.Char.UnicodeChar = L' ';
        std::fill_n(*buffer, width * height, default);
    }

    void draw() const
    {
        if (wcharMode)
            ptr->fillConsoleW(*buffer);
        else
            ptr->fillConsole(*buffer);
    }

    template <typename ToDraw>
    ToDraw add(short width, short height, short x, short y)
    {
        return ToDraw{width, height, buffer, y, x};
    }

    template<typename ToDraw>
    ToDraw add()
    {
        return ToDraw{ width, height, buffer, 0, 0 };
    }

    /*Specialization*/
    template<>
    Video add()
    {
        return Video{ width, height, buffer, 0 ,0, *this };
    }
    template <>
    Video add(short width, short height, short x, short y)
    {
        return Video{ width, height, buffer, y, x, *this };
    }

    template<typename Left, typename Right>
    std::pair<Left, Right> divide(RectangleArea::Divisor d, short pos)
    {
        if (d == RectangleArea::Divisor::Horizontal)
            return{
            Left{pos, height, buffer, 0, 0},
            Right{width - pos - 1, height, buffer, 0, pos + 1}
        };
        else    //vertical divisor
            return {
            Left{width, pos, buffer, 0, 0},
            Right{width, height - pos - 1, buffer, pos + 1, 0}
        };
    }

    void setWcharMode() { wcharMode = true; }
    void setAsciiMode() { wcharMode = false; }
};

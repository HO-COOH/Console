#pragma once
#include "TextVideo.h"
#include "BufferObject.hpp"
#include <mutex>
class ConsoleEngine
{
private:
    short width;
    short height;
    Console* ptr;
    bool wcharMode;
    mutable std::mutex mut;
    bool multiThreadingMode = false;
public:
    BufferObject buffer;
    ConsoleEngine(Console& window, bool wcharMode=false) :width(window.getWidth()), height(window.getHeight()), buffer(height, width), ptr(&window), wcharMode(wcharMode) 
    {
        CHAR_INFO init;
        init.Attributes = 7;
        if (wcharMode)
            init.Char.AsciiChar = ' ';
        else
            init.Char.UnicodeChar = L' ';
        std::fill_n(*buffer, width * height, init);
    }

    void draw() const
    {
        if (multiThreadingMode)
        {
            std::lock_guard lk{ mut };
            if (wcharMode)
                ptr->fillConsoleW(*buffer);
            else
                ptr->fillConsole(*buffer);
        }
        else/*[[likely]]*/
        {
            if (wcharMode)
                ptr->fillConsoleW(*buffer);
            else
                ptr->fillConsole(*buffer);
        }
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
    template<>
    PictureEngine add()
    {
        return PictureEngine{ width, height, buffer, 0 ,0, *this };
    }
    template <>
    PictureEngine add(short width, short height, short x, short y)
    {
        return PictureEngine{ width, height, buffer, y, x, *this };
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
    void setMultiThreadingMode(){ multiThreadingMode = true; }
    void setSingleThreadingMode() { multiThreadingMode = false; }
};

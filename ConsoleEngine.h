#pragma once
#include "Console.h"

struct BufferObject
{
    CHAR_INFO* buffer;
    short rows;
    short columns;
    BufferObject(short rows, short columns) :buffer(new CHAR_INFO[static_cast<size_t>(rows)* columns]), rows(rows), columns(columns) {}
    ~BufferObject() { delete[] buffer; }
    CHAR_INFO& operator()(short row, short column) { return buffer[row * columns + column]; }
    CHAR_INFO* operator*() const { return buffer; }
};

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
    friend class VideoEngine;
};

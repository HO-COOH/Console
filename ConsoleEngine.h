#pragma once
#include "Console.h"
#include "Shape.h"

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
public:
    ConsoleEngine(Console& window) :width(window.getWidth()), height(window.getHeight()), buffer(height, width), ptr(&window) {
        CHAR_INFO default;
        default.Attributes = 7;
        default.Char.AsciiChar = ' ';
        std::fill_n(*buffer, width * height, default);
    }
    void draw() const
    {
        ptr->fillConsole(*buffer);
    }

    template <typename ToDraw>//, typename = std::enable_if_t<std::is_base_of_v<ToDraw, RectangleArea>>>
    ToDraw add()
    {
        return ToDraw{width, height, buffer, 0, 0};
    }

    ~ConsoleEngine() = default;
};

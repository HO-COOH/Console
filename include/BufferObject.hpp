#pragma once
#include "Console.h"
struct BufferObject
{
    CHAR_INFO* buffer;
    short rows;
    short columns;
    BufferObject(short rows, short columns) :buffer(new CHAR_INFO[static_cast<size_t>(rows) * columns]), rows(rows), columns(columns) {}
    ~BufferObject() { delete[] buffer; }
    CHAR_INFO& operator()(short row, short column) { return buffer[row * columns + column]; }
    CHAR_INFO* operator*() const { return buffer; }
};
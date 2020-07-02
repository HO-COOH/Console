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

    class BufferObjectIterator
    {
        CHAR_INFO* m_ptr;
    public:
        BufferObjectIterator(CHAR_INFO* ptr) :m_ptr(ptr) {}
        CHAR_INFO& operator*() { return *m_ptr; }
        CHAR_INFO const& operator*() const { return *m_ptr; }
        CHAR_INFO* operator->() { return m_ptr; }
        BufferObjectIterator& operator++() { ++m_ptr; return *this; }
        BufferObjectIterator& operator--() { --m_ptr; return *this; }
        bool operator==(BufferObjectIterator const& rhs) { return m_ptr == rhs.m_ptr; }
        bool operator!=(BufferObjectIterator const& rhs) { return m_ptr != rhs.m_ptr; }
    };

    BufferObjectIterator begin() { return { buffer }; }
    BufferObjectIterator end() { return { buffer + static_cast<size_t>(rows) * columns }; }
};
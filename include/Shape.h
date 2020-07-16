#pragma once
#include <utility>
#include <string>
#include <iostream>
#include <algorithm>
#include <deque>
#include "Console.h"
#include "BufferObject.hpp"

class Drawable
{
protected:
    BufferObject& buffer;
    short width;
    short height;
    short starting_row;
    short starting_col;
    char& operator()(short row, short col)const { return buffer(row+starting_row, col+starting_col).Char.AsciiChar;}
    CHAR_INFO& at(short row, short col) const { return buffer(row + starting_row, col + starting_col); }
    //auto begin() { return BufferObject::BufferObjectIterator{ &at(0,0) }; }
    //auto end() { return BufferObject::BufferObjectIterator{ &at(height, width) }; }
    void fill(char c, Color color, BackgroundColor bg=BackgroundColor::DEFAULT)
    {
        CHAR_INFO fillChar;
        fillChar.Char.AsciiChar = c;
        fillChar.Attributes = static_cast<WORD>(color) | static_cast<WORD>(bg);
        for (auto i = 0; i < height; ++i)
            std::fill(BufferObject::BufferObjectIterator{ &at(i,0) }, BufferObject::BufferObjectIterator{ &at(i, width) }, fillChar);
    }
    void fill(wchar_t c, Color color, BackgroundColor bg = BackgroundColor::DEFAULT)
    {
        CHAR_INFO fillChar;
        fillChar.Char.UnicodeChar = c;
        fillChar.Attributes = static_cast<WORD>(color) | static_cast<WORD>(bg);
        for (auto i = 0; i < height; ++i)
            std::fill(BufferObject::BufferObjectIterator{ &at(i,0) }, BufferObject::BufferObjectIterator{ &at(i, width) }, fillChar);
    }
public:
    Drawable(BufferObject& buffer, short width, short height,  short starting_row, short starting_col)
        :   buffer(buffer),
            width(width),
            height(height),
            starting_row(starting_row),
            starting_col(starting_col) { }
    void setColor(Color color, bool text_intensify = true, BackgroundColor bgColor = BackgroundColor::DEFAULT, bool bg_intensity = false);
    virtual void draw() const = 0;

    //class BufferIterator
    //{
    //    CHAR_INFO* m_ptr;
    //    short width;
    //    short height;
    //    short currentX{};
    //    short currentY{};
    //public:
    //    BufferIterator(CHAR_INFO* ptr, Drawable const& drawable) :m_ptr(ptr), width(drawable.width), height(drawable.height) {}
    //    CHAR_INFO& operator*() { return *m_ptr; }
    //    CHAR_INFO const& operator*() const { return *m_ptr; }
    //    CHAR_INFO* operator->() { return m_ptr; }
    //    BufferIterator& operator++() 
    //    {
    //        if(++currentX)
    //        ++m_ptr;
    //        return *this; 
    //    }
    //    BufferIterator& operator--() 
    //    { 
    //        --m_ptr;
    //        return *this;
    //    }
    //    bool operator==(BufferIterator const& rhs) { return m_ptr == rhs.m_ptr; }
    //    bool operator!=(BufferIterator const& rhs) { return m_ptr != rhs.m_ptr; }
    //};
};


class RectangleArea:public Drawable
{
protected:
    char up=' ';
    char down=' ';
    char left=' ';
    char right=' ';
    void drawBorder() const;
    short startRow() const { return up == ' ' ? 0 : 1; }
    short startCol() const { return left == ' ' ? 0 : 1; }
    short endRow() const { return down == ' ' ? height - 2 : height - 1; }
    short endCol() const { return right == ' ' ? width - 2 : width - 1; }
    short getUsableWidth() const { return width - (left == ' ' ? 0 : 1) - (right == ' ' ? 0 : 1); }
    short getUsableHeight() const { return height - (up == ' ' ? 0 : 1) - (down == ' ' ? 0 : 1); }
public:
    virtual void draw() const override { drawBorder(); }
    
    enum class Divisor
    {
        Horizontal,
        Vertical
    };

    RectangleArea(BufferObject& buffer, short width, short height,  short starting_row, short starting_col)
        : Drawable(buffer, width, height, starting_row, starting_col) { }
    
    std::pair<RectangleArea, RectangleArea> divide(Divisor d, short pos, char divisor=0);
    
    void setBorder(char up, char down, char left, char right);

    friend std::ostream& operator<<(std::ostream& os, RectangleArea const& r)
    {
        os << "width = " << r.width << " height= " << r.height;
        return os;
    }

    template<typename Widget, typename...Args>
    Widget add(Args&&... args) { return Widget{buffer, getUsableWidth(), getUsableHeight(), starting_row+startRow(), starting_col+startCol(), args...}; }

};

class TextBox: public RectangleArea
{
public:
    TextBox(BufferObject& buffer, short width, short height,  short starting_row, short starting_col) : RectangleArea(buffer, width, height, starting_row, starting_col) {}
    void setText(std::string const& text);
};

class ScrollView: public RectangleArea
{
    std::deque<std::string> textQueue;
    void pop();
public:
    ScrollView(BufferObject& buffer, short width, short height, short starting_row, short starting_col):RectangleArea(buffer, width, height, starting_row, starting_col){}
    void push(std::string&& text) { textQueue.push_back(std::move(text)); pop(); }
    void draw() const override;
};


class ProgressBar:public RectangleArea
{
   char finished='=';
   char rest='-';
   char arrow='>';
   short m_percent = 0;
   bool show_percentage = true;
public:
   ProgressBar(BufferObject &buffer, short width, short height, short starting_row, short starting_col)
       : RectangleArea(buffer, width, height, starting_row, starting_col) {}

   void setPercentage(short percent)
   {
       if (percent >= 0 && percent <= 100)
           m_percent = percent;
       draw();
   }
   void showPercentage() { show_percentage = true; }
   void hidePercentage() { show_percentage = false; }
   void draw() const override;
};


class Form: public RectangleArea
{
private:
    std::vector<std::string_view> heading;
    short m_rows = 1;
    short m_cols = 1;
public:
    Form(BufferObject& buffer, short width, short height, short starting_row, short starting_col)
        : RectangleArea(buffer, width, height, starting_row, starting_col) {}

    /**
     * @brief: Set the size of the forms, size is including the heading rows 
     */
    void setSize(short rows, short columns)
    {
        m_rows = rows;
        m_cols = columns;
    }

    class FormCell :public RectangleArea
    {
    public:
        FormCell(BufferObject& buffer, short width, short height, short starting_row, short starting_col)
            :RectangleArea(buffer, width, height, starting_row, starting_col) {}
        
        void setText(std::string_view text);
    };

    void setHeading(std::vector<std::string_view> headings) { heading = std::move(headings); }

    FormCell& operator()(short row, short col);
    
    void draw() const override;

};

class Menu :public RectangleArea
{
public:
    void setInactiveBackgroundColor();
    void setActiveBackgroundColor();

};
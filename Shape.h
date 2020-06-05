#pragma once
#include <utility>
#include <string>
#include <iostream>
#include <algorithm>
#include "ConsoleEngine.h"

class Drawable
{
protected:
    short width;
    short height;
    BufferObject& buffer;
    short starting_row;
    short starting_col;
    virtual char& operator()(short row, short col)const { return buffer(row+starting_row, col+starting_col).Char.AsciiChar;}
    virtual CHAR_INFO& at(short row, short col) const { return buffer(row + starting_row, col + starting_col); }
public:
    Drawable(short width, short height, BufferObject& buffer, short starting_row, short starting_col)
        :   width(width),
            height(height),
            buffer(buffer),
            starting_row(starting_row),
            starting_col(starting_col) { }
    void setColor(Color color, bool text_intensify = true, BackgroundColor bgColor = BackgroundColor::DEFAULT, bool bg_intensity = false);
    virtual void draw() const = 0;
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
public:
    virtual void draw() const override { drawBorder(); }
    
    enum class Divisor
    {
        Horizontal,
        Vertical
    };

    RectangleArea(short width, short height, BufferObject& buffer, short starting_row, short starting_col)
        : Drawable(width, height, buffer, starting_row, starting_col) { }
    
    std::pair<RectangleArea, RectangleArea> divide(Divisor d, short pos, char divisor=0);
    
    void setBorder(char up, char down, char left, char right);

    friend std::ostream& operator<<(std::ostream& os, RectangleArea const& r)
    {
        os << "width = " << r.width << " height= " << r.height;
        return os;
    }

    template<typename Widget>
    Widget add() { return Widget{width, height, buffer, starting_row, starting_col}; }
};

class TextBox: public RectangleArea
{
public:
    TextBox(short width, short height, BufferObject& buffer, short starting_row, short starting_col) : RectangleArea(width, height, buffer, starting_row, starting_col) {}
    void setText(std::string const& text);
};

//class ScrollView: public RectangleArea
//{
//public:
//    ScrollView(short width, short height, CHAR_INFO* buffer_location):RectangleArea(width, height, buffer_location){}
//    void push(std::string const &text);
//};
//

class ProgressBar:public RectangleArea
{
   char finished='=';
   char rest='-';
   char arrow='>';
   short m_percent = 0;
   bool show_percentage = true;
public:
   ProgressBar(short width, short height, BufferObject &buffer, short starting_row, short starting_col)
       : RectangleArea(width, height, buffer, starting_row, starting_col) {}

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
    Form(short width, short height, BufferObject& buffer, short starting_row, short starting_col)
        : RectangleArea(width, height, buffer, starting_row, starting_col) {}

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
        FormCell(short width, short height, BufferObject& buffer, short starting_row, short starting_col)
            :RectangleArea(width, height, buffer, starting_row, starting_col) {}
        
        void setText(std::string_view text);
    };

    void setHeading(std::vector<std::string_view> headings) { heading = std::move(headings); }

    FormCell& operator()(short row, short col);
    
    void draw() const override;

};


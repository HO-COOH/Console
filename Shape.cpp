#include "Shape.h"
#include <iostream>
#include <algorithm>

void RectangleArea::drawBorder() const
{
    if (up != ' ')
    {
        for (auto i = 0; i < width; ++i)
            (*this)(0, i) = up;
    }
    if (down != ' ')
    {
        for (auto i = 0; i < width; ++i)
            (*this)(height - 1, i) = down;
    }
    if (left != ' ')
    {
        for (auto i = 0; i < height; ++i)
            (*this)(i, 0) = left;
    }
    if (right != ' ')
    {
        for (auto i = 0; i < height; ++i)
            (*this)(i, width - 1) = right;
    }
}

void RectangleArea::draw() const
{
    drawBorder();
}

/*
    vertical divider:
       buffer location
      |  
    ******
    ******
      |
      divide here

    horizontal divider:

    ******
    ******
    ****** - divide here
    ******
    ******

*/
std::pair<RectangleArea, RectangleArea> RectangleArea::divide(Divisor d, short pos ,char divisor)
{
    if(d==Divisor::Vertical)
    {
        if(pos>width-2)
        {
            std::cerr << "Invalid divide() parameter!\n";
            pos = (width-2) / 2;
        }
        if(pos==0)
            pos = (width - 2) / 2;
        return {RectangleArea{pos-1, height-2, buffer, starting_row+1, starting_col+1}, RectangleArea{width - pos - 2, height-2, buffer, starting_row+1, pos+1}};
    }
    else
    {
        if(pos>height-2)
        {
            std::cerr << "Invalid divide() parameter!\n";
            pos = (height - 2) / 2;
        }
        if(pos==0)
            pos = (height - 2) / 2;
        return {RectangleArea{width-2, pos-1, buffer, starting_row+1, starting_col+1}, RectangleArea{width-2, height - pos - 2, buffer, pos+1, starting_col+1}};
    }
}

void RectangleArea::setBorder(char up, char down, char left, char right)
{
    this->up = up;
    this->down = down;
    this->left=left;
    this->right = right;
    drawBorder();
}

void TextBox::setText(std::string const& text)
{
    short row = 0, col = 0;
    for (auto c : text)
    {
        (*this)(row, col++) = c;
        if (col == width)
        {
            col = 0;
            ++row;
        }
        if (row == height && col == width)
            break;
    }
}


void ProgressBar::draw() const
{
    (*this)(0, 0) = '[';
    const short margin = show_percentage ? 8 : 5;
    short finished_count = (m_percent /100.f) * (width - margin);
    short rest_count = ((100 - m_percent) /100.f) * (width - margin);
    short j = 1;
    for (short i = 0; i < finished_count; ++i, ++j)
        (*this)(0, j) = finished;
    (*this)(0, j++) = arrow;
    for (short i = 0; i < rest_count; ++i,++j)
        (*this)(0, j) = rest;
    (*this)(0, j++) = ']';
    if (show_percentage)
    {
        char buffer[4];
        sprintf(buffer, "%.3d", m_percent);
        for (short i = 0; i < 3; ++i)    //3 digits to process
            (*this)(0, j++) = (i == 0 && buffer[0] == '0' ?  ' ':buffer[i]);
        (*this)(0, j) = '%';
    }
}

//Form::FormCell Form::operator()(short row, short col)
//{
//    return FormCell();
//}

void Form::draw() const
{
    short headingRows = std::max_element(heading.cbegin(), heading.cend(), [](auto& str1, auto& str2) {return str1.length() < str2.length(); })->length();

}

void Form::FormCell::setText(std::string_view text)
{
}

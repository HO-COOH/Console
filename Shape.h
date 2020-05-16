#include <utility>
class Drawable
{
    short width;
    short height;
public:
    virtual void draw() const = 0;
};


class RectangleArea:Drawable
{
public:
    enum class Divisor
    {
        Horizontal,
        Vertical
    };
    RectangleArea(short width, short height, char upBorder=' ', char downBorder=' ', char leftBorder=' ', char rightBorder=' ');
    std::pair<RectangleArea &, RectangleArea &> divide(Divisor d);
    void setText();
    void draw() const override;
};

class TextBox:RectangleArea
{
};

class ScrollView:RectangleArea
{
};


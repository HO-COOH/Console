#pragma once
#include "Shape.h"
#include <array>
#include <cmath>
#include <utility>



struct Color_T
{
    uint8_t r, g, b;
};

static inline int colorDistance(Color_T lhs, Color_T rhs)
{
    return sqrt(static_cast<int>((static_cast<int>(lhs.r) - rhs.r) * (static_cast<int>(lhs.r) - rhs.r) + (static_cast<int>(lhs.g) - rhs.g) * (static_cast<int>(lhs.g) - rhs.g) + (static_cast<int>(lhs.b) - rhs.b) * (static_cast<int>(lhs.b) - rhs.b)));
}

constexpr const std::array<std::pair<Color, Color_T>, 8> color_list
{
    std::pair{Color::BLACK, Color_T{0,0,0}},
    {Color::RED, {128,0,0}},
    {Color::GREEN, {0,128,0}},
    {Color::BLUE, {0,0,128}},
    {Color::YELLOW, {128,128,0}},
    {Color::CYAN, {0,128,128}},
    {Color::MAGENTA, {128,0,128}},
    {Color::WHITE, {128,128,128}}
};

/**
 * @brief convert individual pixel {R,G,B} to a CHAR_INFO struct using the pre-defined color values
 * @param r
 * @param g
 * @param b
 * @return
*/


//TODO: switch to ffmpeg
//TODO: automatic frame rate detect
class VideoEngine :public RectangleArea
{
    std::string m_fileName;
    ConsoleEngine& m_engine;
public:
    VideoEngine(short width, short height, BufferObject& buffer, short starting_row, short starting_col, std::string fileName, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_fileName(std::move(fileName)), m_engine(engine) {}
    VideoEngine(short width, short height, BufferObject& buffer, short starting_row, short starting_col, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_engine(engine) {}
    VideoEngine(ConsoleEngine& engine) :RectangleArea(0,0,*static_cast<BufferObject*>(nullptr), 0,0), m_engine(engine) {}   //TODO: factor this

    void load(std::string_view fileName) { m_fileName = fileName; }
    void play(unsigned int frameRate);
};

//TODO: here
class PictureEngine
{
public:

};
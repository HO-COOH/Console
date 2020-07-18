#pragma once
#include "Shape.h"
#include <array>
#include <utility>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>

template<typename T>
inline float square(T value) { return static_cast<float>(value) * value; }

struct Color_T
{
    uint8_t r, g, b;
    Color_T operator*(float factor) const
    {
        return { 
            static_cast<uint8_t>(r * factor), 
            static_cast<uint8_t>(g * factor), 
            static_cast<uint8_t>(b * factor) 
        };
    }
    Color_T operator+(Color_T rhs) const
    {
        return { 
            static_cast<uint8_t>(r + rhs.r),
            static_cast<uint8_t>(g + rhs.g),
            static_cast<uint8_t>(b + rhs.b) 
        };
    }
    Color_T operator-(Color_T rhs) const
    {
        return {
            static_cast<uint8_t>(((int)r - rhs.r) <0? 0 : r - rhs.r),
            static_cast<uint8_t>(((int)g - rhs.g) < 0? 0 : g - rhs.g),
            static_cast<uint8_t>(((int)b - rhs.b) <0? 0: b-rhs.b) 
        };
    }
    /**
     * @brief return a value within [0.0-1.0] scale representing the intensity of the color
    */
    float getIntensity() const
    {
        return (float(r) + float(g) + float(b)) / 3.0f /(float)(UINT8_MAX) ;
    }
    int getDistance(Color_T rhs) const
    {
        return sqrt((square((float)r-rhs.r) + square((float)g-rhs.g) + square((float)b-rhs.b))/3);
    }
};


//TODO: switch to ffmpeg
//TODO: automatic frame rate detect
class ConsoleEngine;    //forward declaration
class BufferObject;
class Video :public RectangleArea
{
    std::string m_fileName;
    ConsoleEngine& m_engine;
    /*testing only*/
public:
    void drawColorWall();
    bool is_ready = false;
    friend class VideoEngine;
    Video(BufferObject& buffer, short width, short height, short starting_row, short starting_col, std::string fileName, ConsoleEngine& engine)
        :RectangleArea(buffer, width, height, starting_row, starting_col), m_fileName(std::move(fileName)), m_engine(engine) {}
    Video(BufferObject& buffer, short width, short height, short starting_row, short starting_col, ConsoleEngine& engine)
        :RectangleArea(buffer, width, height, starting_row, starting_col), m_engine(engine) {}
    //VideoEngine(ConsoleEngine& engine) :RectangleArea(0,0,*static_cast<BufferObject*>(nullptr), 0,0), m_engine(engine) {}   //TODO: factor this
    using RectangleArea::at;

    void load(std::string_view fileName);
    void play(unsigned int frameRate);
    unsigned int getFrameRate();
};


#include <atomic>
#include <functional>
class VideoEngine
{
    ConsoleEngine& m_engine;
    std::vector<std::thread> m_instances;
    std::vector<std::atomic_bool> finishedFlags;

    std::atomic<int> processedFrames;
    std::shared_mutex drawing_mutex;
    std::condition_variable_any cond;

    void draw();
    bool is_all_finished() const;
public:
    VideoEngine(std::vector<std::reference_wrapper<Video>> instances);
    ~VideoEngine();
};

//TODO: here
class PictureEngine:public RectangleArea
{
    std::string m_fileName;
    ConsoleEngine& m_engine;
public:
    PictureEngine(BufferObject& buffer, short width, short height, short starting_row, short starting_col, std::string fileName, ConsoleEngine& engine)
        :RectangleArea(buffer, width, height, starting_row, starting_col), m_fileName(std::move(fileName)), m_engine(engine) {}

    PictureEngine(BufferObject& buffer, short width, short height, short starting_row, short starting_col, ConsoleEngine& engine)
        :RectangleArea(buffer, width, height, starting_row, starting_col), m_engine(engine) {}

    void load(std::string_view fileName);
    void load();
};


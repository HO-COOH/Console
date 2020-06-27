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
            static_cast<uint8_t>(b * factor) };
    }
    Color_T operator+(Color_T rhs) const
    {
        return { 
            static_cast<uint8_t>(r + rhs.r),
            static_cast<uint8_t>(g + rhs.g),
            static_cast<uint8_t>(b + rhs.b) };
    }
    Color_T operator-(Color_T rhs) const
    {
        return {
            static_cast<uint8_t>(((int)r - rhs.r) <0? 0 : r - rhs.r),
            static_cast<uint8_t>(((int)g - rhs.g) < 0? 0 : g - rhs.g),
            static_cast<uint8_t>(((int)b - rhs.b) <0? 0: b-rhs.b) };
    }
    float getIntensity() const
    {
        //return sqrt((square(r) + square(g) + square(b)) / 3) / UINT8_MAX;
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
    Video(short width, short height, BufferObject& buffer, short starting_row, short starting_col, std::string fileName, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_fileName(std::move(fileName)), m_engine(engine) {}
    Video(short width, short height, BufferObject& buffer, short starting_row, short starting_col, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_engine(engine) {}
    //VideoEngine(ConsoleEngine& engine) :RectangleArea(0,0,*static_cast<BufferObject*>(nullptr), 0,0), m_engine(engine) {}   //TODO: factor this
    using RectangleArea::at;

    void load(std::string_view fileName);
    void play(unsigned int frameRate);
    unsigned int getFrameRate();
};

#include <queue>
namespace internal
{
    class VideoEvent
    {
        mutable std::mutex mut;
        unsigned event_counter{ 0 };
        std::condition_variable data_cond;
        unsigned m_count;
    public:
        VideoEvent(unsigned count):m_count(count) {}
        void push(bool flag)
        {
            {
                std::lock_guard lk{ mut };
                ++event_counter;
            }
            data_cond.notify_one();
        }
        void wait_and_pop()
        {
            std::unique_lock lk{ mut };
            data_cond.wait(lk, [&] {return event_counter>=m_count; });
            event_counter = 0;
            data_cond.notify_all();
        }
        void wait_for_empty()
        {
            std::unique_lock lk{ mut };
            data_cond.wait(lk, [&] {return event_counter == 0; });
            data_cond.notify_all();
        }
    };
}

//#include <stack>
//class VideoEngine
//{
//    ConsoleEngine& m_engine;
//    std::vector<std::thread> m_instances;
//    
//    std::shared_mutex ready_mutex;
//    std::condition_variable_any ready_cond;
//
//    std::shared_mutex drawing_mutex;
//    std::condition_variable_any drawing_cond;
//
//    std::deque<std::atomic_bool> finished_flags;
//
//    void draw();
//    bool is_all_finished() const;
//public:
//    VideoEngine(std::vector<Video*> instances);
//    ~VideoEngine();
//};
class VideoEngine
{
    ConsoleEngine& m_engine;
    internal::VideoEvent queue;
    std::vector<std::thread> m_instances;
    std::deque<std::atomic_bool> finished_flags;
    std::shared_mutex drawing_mutex;
    void draw();
    bool is_all_finished() const;
public:
    VideoEngine(std::vector<Video*> instances);
    ~VideoEngine();
};

//TODO: here
class PictureEngine:public RectangleArea
{
    std::string m_fileName;
    ConsoleEngine& m_engine;
public:
    PictureEngine(short width, short height, BufferObject& buffer, short starting_row, short starting_col, std::string fileName, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_fileName(std::move(fileName)), m_engine(engine) {}

    PictureEngine(short width, short height, BufferObject& buffer, short starting_row, short starting_col, ConsoleEngine& engine)
        :RectangleArea(width, height, buffer, starting_row, starting_col), m_engine(engine) {}

    void load(std::string_view fileName);
    void load();
};


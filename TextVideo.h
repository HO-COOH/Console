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


struct Color_T
{
    uint8_t r, g, b;
};

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


//TODO: switch to ffmpeg
//TODO: automatic frame rate detect
class ConsoleEngine;    //forward declaration
class BufferObject;
class Video :public RectangleArea
{
    std::string m_fileName;
    ConsoleEngine& m_engine;
public:
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
};

#include <queue>
namespace internal
{
    class Threadsafe
    {
        mutable std::mutex mut;
        std::deque<bool> data;
        std::condition_variable data_cond;
        unsigned m_count;
    public:
        Threadsafe(unsigned count):m_count(count) {}
        void push(bool flag)
        {
            {
                std::lock_guard lk{ mut };
                data.push_back(flag);
            }
            data_cond.notify_one();
        }
        void wait_and_pop()
        {
            std::unique_lock lk{ mut };
            data_cond.wait(lk, [&] {return data.size()>=m_count; });
            data.clear();
            data_cond.notify_all();
        }
        void wait_for_empty()
        {
            std::unique_lock lk{ mut };
            data_cond.wait(lk, [&] {return data.empty(); });
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
    internal::Threadsafe queue;
    std::vector<std::thread> m_instances;
    std::deque<std::atomic_bool> finished_flags;
    void draw();
    bool is_all_finished() const;
public:
    VideoEngine(std::vector<Video*> instances);
    ~VideoEngine();
};

//TODO: here
class PictureEngine
{
public:

};
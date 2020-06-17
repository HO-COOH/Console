#include "TextVideo.h"
#include "TimedEvent.hpp"
#include <opencv2/opencv.hpp>
#include "ConsoleEngine.h"
#include <cmath>
#include <filesystem>

static inline int colorDistance(Color_T lhs, Color_T rhs)
{
    return sqrt(static_cast<int>((static_cast<int>(lhs.r) - rhs.r) * (static_cast<int>(lhs.r) - rhs.r) + (static_cast<int>(lhs.g) - rhs.g) * (static_cast<int>(lhs.g) - rhs.g) + (static_cast<int>(lhs.b) - rhs.b) * (static_cast<int>(lhs.b) - rhs.b)));
}

/**
 * @brief convert individual pixel {R,G,B} to a CHAR_INFO struct using the pre-defined color values
 * @param r
 * @param g
 * @param b
*/
inline static CHAR_INFO to_text(uint8_t r, uint8_t g, uint8_t b)
{
    std::array<int, color_list.size()> distance;
    for (auto i = 0; i < color_list.size(); ++i)
        distance[i] = colorDistance(Color_T{ r,g,b }, color_list[i].second);
    auto result = color_list[std::distance(distance.cbegin(), std::min_element(distance.cbegin(), distance.cend()))].first;
    CHAR_INFO c;
    c.Attributes = result == Color::BLACK ? 0 : static_cast<WORD>(result) | FOREGROUND_INTENSITY;
    c.Char.UnicodeChar = static_cast<wchar_t>(Shade::Dark); //TODO: implement algorithm here
    //c.Char.UnicodeChar = L'Ѿ';    //test for wchar
    return c;
}

void Video::load(std::string_view fileName) 
{ 
    if (std::filesystem::directory_entry{ std::filesystem::path{ fileName } }.exists())
    {
        m_fileName = fileName;
        is_ready = true;
    }
    else
        std::cerr << "File: " << fileName << " doesn't exist!\n";
}

void Video::play(unsigned int frameRate)
{
    cv::VideoCapture video{ m_fileName };
    cv::Mat frame, resized;
    ScopedTimer t{ static_cast<unsigned int>(1.0 / frameRate * 1000 * 1000) };
    while (true)
    {
        video >> frame;
        if (frame.empty())
            break;
        cv::resize(frame, resized, { width, height });
        for (auto i = 0; i < height; ++i)
        {
            for (auto j = 0; j < width; ++j)
            {
                auto pixel = resized.at<cv::Vec3b>(i, j);
                buffer(i, j) = to_text(pixel[2], pixel[1], pixel[0]);
            }
        }
        m_engine.draw();
        t.wait();
    }
}

/*  
draw thread:
    while(true)
        for each ready_mutex:
            ready_mutex.lock()
        //all finished
        engine.draw()
        for each ready_mutex:
            ready_mutex.unlock


*/

//VideoEngine::VideoEngine(std::vector<Video*> instances) : m_engine(instances.front()->m_engine)
//{
//    unsigned id = 0;
//    for (auto instance : instances)
//    {
//        if (instance->is_ready)
//        {
//            finished_flags.emplace_back(false);
//            m_instances.emplace_back
//            (
//                [&, instance]
//                {
//                    unsigned m_id = id;
//                    cv::VideoCapture video{ instance->m_fileName };
//                    cv::Mat frame, resized;
//                    ScopedTimer t{ static_cast<unsigned int>(1.0 / 30 * 1000 * 1000) }; //refactor here
//                    while (true)
//                    {
//                        {
//                            ready_mutex.lock_shared();
//                            video >> frame;
//                            if (frame.empty())
//                                break;
//                            cv::resize(frame, resized, { instance->width, instance->height });
//                            for (auto i = 0; i < instance->height; ++i)
//                            {
//                                for (auto j = 0; j < instance->width; ++j)
//                                {
//                                    auto pixel = resized.at<cv::Vec3b>(i, j);
//                                    instance->buffer(i, j) = to_text(pixel[2], pixel[1], pixel[0]);
//                                }
//                            }
//                            ready_mutex.unlock_shared();
//                            ready_cond.notify_one();
//                        }
//                        {
//                            std::shared_lock lk(drawing_mutex);
//                            drawing_cond.wait(lk);
//                            t.wait();
//                        }
//                    }
//                    finished_flags[m_id] = true;
//                }
//                );
//            ++id;
//        }
//    }
//    draw();
//}
//
//VideoEngine::~VideoEngine()
//{
//    for (auto& instance : m_instances)
//        instance.join();
//}
//
//bool VideoEngine::is_all_finished() const
//{
//    for (const auto& flag : finished_flags)
//    {
//        if (!flag)
//            return false;
//    }
//    return true;
//}
//
//void VideoEngine::draw()
//{
//    while (!is_all_finished())
//    {
//        {
//            std::unique_lock lk{ ready_mutex };
//            std::shared_lock drawing_lock{ drawing_mutex };
//            drawing_cond.wait(drawing_lock);
//            m_engine.draw();
//        }
//    }
//}

void VideoEngine::draw()
{
    while (!is_all_finished())
    {
        queue.wait_and_pop();
        m_engine.draw();
    }
}

bool VideoEngine::is_all_finished() const
{
     for (const auto& flag : finished_flags)
     {
         if (!flag)
             return false;
     }
     return true;
}

namespace {
    unsigned ready_count(std::vector<Video*> const& instances)
    {
        unsigned count = 0;
        for (auto instance : instances)
        {
            if (instance->is_ready)
                ++count;
        }
        return count;
    }
}

VideoEngine::VideoEngine(std::vector<Video*> instances): m_engine(instances.front()->m_engine), queue(ready_count(instances))
{
    unsigned id = 0;
    for (auto instance : instances)
    {
        if (instance->is_ready)
        {
            finished_flags.emplace_back(false);
            m_instances.emplace_back
            (
                [&, instance]
                {
                    auto m_id = id;
                    cv::VideoCapture video{ instance->m_fileName };
                    cv::Mat frame, resized;
                    ScopedTimer t{ static_cast<unsigned int>(1.0 / 30 * 1000 * 1000) }; //refactor here
                    while (true)
                    {
                        video >> frame;
                        if (frame.empty())
                            break;
                        cv::resize(frame, resized, { instance->width, instance->height });
                        for (auto i = 0; i < instance->height; ++i)
                        {
                            for (auto j = 0; j < instance->width; ++j)
                            {
                                auto pixel = resized.at<cv::Vec3b>(i, j);
                                instance->at(i, j) = to_text(pixel[2], pixel[1], pixel[0]);
                            }
                        }
                        queue.push(true);
                        t.wait();
                    }
                    finished_flags[m_id] = true;
                }
                );
            ++id;
        }
    }
    draw();
}

VideoEngine::~VideoEngine()
{
    for (auto& instance : m_instances)
        instance.join();
}

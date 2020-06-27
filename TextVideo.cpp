#include <opencv2/opencv.hpp>
#include "TextVideo.h"
#include "TimedEvent.hpp"
#include "ConsoleEngine.h"
#include <cmath>
#include <filesystem>
#include <array>

static constexpr const std::array<std::pair<Color, Color_T>, 8> color_list
{
    std::pair<Color, Color_T>{Color::BLACK, Color_T{0,0,0}},
    {Color::RED,        {255,0,0}},
    {Color::GREEN,      {0,255,0}},
    {Color::BLUE,       {0,0,255}},
    {Color::YELLOW,     {255,255,0}},
    {Color::CYAN,       {0,255,255}},
    {Color::MAGENTA,    {255,0,255}},
    {Color::WHITE,      {255,255,255}}
};

static constexpr const std::array<std::pair<BackgroundColor, Color_T>, 8> bgColor_list
{
    std::pair{BackgroundColor::DEFAULT, Color_T{0,0,0}},
    {BackgroundColor::RED,          {255,0,0}},
    {BackgroundColor::GREEN,        {0,255,0}},
    {BackgroundColor::BLUE,         {0,0,255}},
    {BackgroundColor::YELLOW,       {255,255,0}},
    {BackgroundColor::CYAN,         {0,255,255}},
    {BackgroundColor::MAGENTA,      {255,0,255}},
    {BackgroundColor::WHITE,        {255,255,255}}
};

static constexpr const std::array<std::pair<Shade, float>, 5> shade_list
{
    std::pair{Shade::Full, 1.0f},
    {Shade::Dark, 0.75f},
    {Shade::Medium, 0.5f},
    {Shade::Light, 0.25f},
    {Shade::None, 0.0f}
};


/**
 * @brief convert individual pixel {R,G,B} to a CHAR_INFO struct using the pre-defined color values
 * @param r
 * @param g
 * @param b
*/
#ifdef WINDOWS

static inline std::pair<Shade, float> getShade(Color_T pixel)
{
    const auto lightness = pixel.getIntensity();
    if (lightness <= 0.2)
        return { Shade::None,0.0f };
    else if (lightness <= 0.4)
        return { Shade::Light, 0.25f };
    else if (lightness <= 0.6)
        return { Shade::Medium, 0.5f } ;
    else if (lightness <= 0.8)
        return { Shade::Dark, 0.75f };
    else
        return { Shade::Full, 1.0f };
}

static inline CHAR_INFO to_text(const Color_T pixel)
{
    /*Extremely CPU intensive */

    //std::array<int, color_list.size()*bgColor_list.size()*shade_list.size()> distance;
    //for (auto i = 0, index=0; i < color_list.size(); ++i)
    //{
    //    for (auto j = 0; j < bgColor_list.size(); ++j)
    //    {
    //        for(auto k=0; k<shade_list.size(); ++k,++index)
    //            distance[index] = colorDistance(
    //                Color_T{ r,g,b }, color_list[i].second * shade_list[k].second + bgColor_list[j].second * (1-shade_list[k].second));
    //    }
    //}
    //auto index = std::distance(distance.cbegin(), std::min_element(distance.cbegin(), distance.cend()));
    //auto color = color_list[index/bgColor_list.size()/shade_list.size()].first;
    //auto bgColor = bgColor_list[index%(bgColor_list.size()*shade_list.size())/bgColor_list.size()].first;
    //auto shade = shade_list[index % shade_list.size()].first;
    
    const auto [shade, shade_factor] = getShade(pixel);
    std::array<int, color_list.size()> distance;
    for (auto i = 0; i < color_list.size(); ++i)
        distance[i] = pixel.getDistance(color_list[i].second * shade_factor);
    const auto [color, color_value] = color_list[std::distance(distance.cbegin(), std::min_element(distance.cbegin(), distance.cend()) )];
    
    auto bgColor = BackgroundColor::DEFAULT;
    bool bgIntensify = false;
    if (shade != Shade::Full && shade != Shade::None)
    {
        std::array<int, bgColor_list.size()> bgDistance;
        const auto remain_color = pixel - color_value * shade_factor;
        for (auto i = 0; i < bgColor_list.size(); ++i)
            bgDistance[i] = remain_color.getDistance(bgColor_list[i].second * (1 - shade_factor));
        bgColor = bgColor_list[std::distance(bgDistance.cbegin(), std::min_element(bgDistance.cbegin(), bgDistance.cend()))].first;
        bgIntensify = (remain_color.getIntensity() >= 128);
    }

    CHAR_INFO c;
    c.Attributes = static_cast<WORD>(color) | static_cast<WORD>(bgColor) | FOREGROUND_INTENSITY;
    /*if (shade==Shade::Full || shade==Shade::Dark) c.Attributes |= FOREGROUND_INTENSITY;*/
    if (bgIntensify) c.Attributes |= BACKGROUND_INTENSITY;
    c.Char.UnicodeChar = static_cast<wchar_t>(shade);
    
    return c;
}
#endif

void Video::drawColorWall()
{
    //loop color
    CHAR_INFO c;

    for (auto i = 0; i < color_list.size(); ++i)
    {
        //loop bgColor
        for (auto j = 0, index=0; j < bgColor_list.size(); ++j)
        {
            for (auto k = 0; k < shade_list.size(); ++k, ++index)
            {
                buffer(i, index).Attributes= static_cast<WORD>(color_list[i].first) | static_cast<WORD>(bgColor_list[j].first) | FOREGROUND_INTENSITY;
                buffer(i, index).Char.UnicodeChar = static_cast<wchar_t>(shade_list[k].first);
            }
        }
    }
    m_engine.draw();
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
                buffer(i, j) = to_text(Color_T{ pixel[2], pixel[1], pixel[0] });
            }
        }
        t.wait();
        m_engine.draw();
    }
}

unsigned int Video::getFrameRate()
{
    //TODO: here
    return 30;
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
        {
            drawing_mutex.lock();
            m_engine.draw();
            drawing_mutex.unlock();
        }
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
//
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
                   
                        {
                            drawing_mutex.lock_shared();
                            for (auto i = 0; i < instance->height; ++i)
                            {
                                for (auto j = 0; j < instance->width; ++j)
                                {
                                    auto pixel = resized.at<cv::Vec3b>(i, j);
                                    instance->at(i, j) = to_text(Color_T{ pixel[2], pixel[1], pixel[0] });
                                }
                            }
                            drawing_mutex.unlock_shared();
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

void PictureEngine::load(std::string_view fileName)
{
    m_fileName = fileName;
    load();
}

void PictureEngine::load()
{
    if (std::filesystem::directory_entry{ std::filesystem::path{ m_fileName } }.exists())
    {
        auto original = cv::imread(m_fileName);
        cv::Mat resized;
        cv::resize(original, resized, { width, height });
        for (auto i = 0; i < height; ++i)
        {
            for (auto j = 0; j < width; ++j)
            {
                auto pixel = resized.at<cv::Vec3b>(i, j);
                buffer(i, j) = to_text(Color_T{ pixel[2], pixel[1], pixel[0] });
            }
        }
        m_engine.draw();
        cv::namedWindow("img");
        cv::imshow("img", resized);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }
    else
        std::cerr << "File: " << m_fileName << " doesn't exist!\n";
}

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
    if (lightness <= 0.4)
        return { Shade::Light, 0.25f };
    if (lightness <= 0.6)
        return { Shade::Medium, 0.5f } ;
    if (lightness <= 0.8)
        return { Shade::Dark, 0.75f };
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
            /*Use pointer dereference instead of array indexing for slightly lower CPU usage */
            auto bufferPtr = &at(i, 0);
            auto resizedRowPtr = resized.ptr<cv::Vec3b>(i);
            for (auto j = 0; j < width; ++j)
            {
                const auto pixel = *resizedRowPtr;
                *bufferPtr = to_text(Color_T{ pixel[2], pixel[1], pixel[0] });
                ++bufferPtr;
                ++resizedRowPtr;
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


void VideoEngine::draw()
{
    while (!is_all_finished())
    {
        std::unique_lock lk{ drawing_mutex };
        cond.wait(lk, [&] {return processedFrames >= m_instances.size(); });
        m_engine.draw();
        processedFrames = 0;
    }
}

bool VideoEngine::is_all_finished() const
{
     for (const auto& flag : finishedFlags)
     {
         if (!flag)
             return false;
     }
     return true;
}

VideoEngine::VideoEngine(std::vector<std::reference_wrapper<Video>> instances): m_engine(instances.front().get().m_engine), finishedFlags(instances.size())
{
    unsigned id = 0;
    for (auto instance : instances)
    {
        if (instance.get().is_ready)
        {
            finishedFlags[id] = false;
            m_instances.emplace_back
            (
                [&, instance]
                {
                    auto m_id = id;
                    cv::VideoCapture video{ instance.get().m_fileName };
                    cv::Mat frame, resized;
                    ScopedTimer t{ static_cast<unsigned int>(1.0 / 30 * 1000 * 1000) }; //refactor here
                    while (true)
                    {
                        video >> frame;
                        if (frame.empty())
                            break;
                        cv::resize(frame, resized, { instance.get().width, instance.get().height });
                   
                        {
                            drawing_mutex.lock_shared();
                            for (auto i = 0; i < instance.get().height; ++i)
                            {
                                for (auto j = 0; j < instance.get().width; ++j)
                                {
                                    auto pixel = resized.at<cv::Vec3b>(i, j);
                                    instance.get().at(i, j) = to_text(Color_T{ pixel[2], pixel[1], pixel[0] });
                                }
                            }
                            drawing_mutex.unlock_shared();
                            ++processedFrames;
                            cond.notify_one();
                        }
                        t.wait();
                    }
                    finishedFlags[m_id] = true;
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

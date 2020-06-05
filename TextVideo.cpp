#include "TextVideo.h"
#include "TimedEvent.hpp"
#include <opencv2/opencv.hpp>

inline static CHAR_INFO to_text(uint8_t r, uint8_t g, uint8_t b)
{
    std::array<int, 8> distance;
    for (auto i = 0; i < color_list.size(); ++i)
        distance[i] = colorDistance(Color_T{ r,g,b }, color_list[i].second);
    auto result = color_list[std::distance(distance.cbegin(), std::min_element(distance.cbegin(), distance.cend()))].first;
    CHAR_INFO c;
    c.Attributes = result == Color::BLACK ? 0 : static_cast<WORD>(result) | FOREGROUND_INTENSITY;
    c.Char.UnicodeChar = static_cast<wchar_t>(Shade::Dark); //TODO: implement algorithm here
    //c.Char.UnicodeChar = L'😀';    //test for wchar
    return c;
}

void VideoEngine::play(unsigned int frameRate)
{
    cv::VideoCapture video{ m_fileName };
    cv::Mat frame, resized;
    ScopedTimer t{ static_cast<unsigned int>(1.0 / frameRate * 1000 * 1000) };
    while (true)
    {
        video >> frame;
        if (frame.empty())
            break;
        cv::resize(frame, resized, { m_engine.width, m_engine.height });
        for (auto i = 0; i < m_engine.height; ++i)
        {
            for (auto j = 0; j < m_engine.width; ++j)
            {
                auto pixel = resized.at<cv::Vec3b>(i, j);
                m_engine.buffer(i, j) = to_text(pixel[0], pixel[1], pixel[2]);
            }
        }
        m_engine.draw();
        t.wait();
    }
}

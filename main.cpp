#include <iostream>
#include "Shape.h"
#include "Console.h"
#include "ConsoleEngine.h"
#include <thread>
#include <atomic>
#include "TextVideo.h"



//void PrintFps()
//{
//    while(true)
//    {
//        SetConsoleTitleA((std::string{"FPS="}+std::to_string(1.0f / (0.000000001 * elapsed_time))).data());
//    }
//}
//
//void TestEngine()
//{
//    Console console;
//    console.printConsoleInfo();
//    std::cin.get();
//    ConsoleEngine eng{console};
//    auto [left, right] = eng.add<RectangleArea>().divide(RectangleArea::Divisor::Vertical, 0);
//    auto [upper_left, bottom_left] = left.divide(RectangleArea::Divisor::Horizontal, 0);
//    auto [upper_right, bottom_right] = right.divide(RectangleArea::Divisor::Horizontal, 0);
//    
//    auto upper_left_bar = upper_left.add<ProgressBar>(); 
//    upper_left_bar.setPercentage(20); 
//    upper_left_bar.setColor(Color::RED);
//    /*auto upper_right_bar    =*/ upper_right.add<ProgressBar>().setPercentage(30);
//    /*auto bottom_left_bar    =*/ bottom_left.add<ProgressBar>().setPercentage(40);
//    /*auto bottom_right_bar   =*/ bottom_right.add<ProgressBar>().setPercentage(50);
//    
//
//    eng.draw();
//    std::cin.get();
//}
//
//void TestConsole()
//{
//    /*Construct a new [Console] class */
//    Console c;
//
//    /*Setting text color using familiar syntax */
//    c << Color::RED << "This is a red text on black background\n";
//    /*Or use traditional member function*/
//    c.set(Color::MAGENTA).writeln("This is magenta text");
//
//    /*Setting background color using the same syntax */
//    c << BackgroundColor::WHITE << "This is a magenta text on white background\n";
//    c.set(BackgroundColor::DEFAULT, false).set(Color::WHITE);
//
//    /*Moving cursor using relative or absolute position */
//    c.moveCursor(4, Direction::RIGHT);
//    c << "Indent text\n";
//    c.moveCursorTo({ 20,10 });
//    c << "Text start at [20,10]\n";
//    c.moveCursorTo(MIDDLE{});
//    
//    /*Erase one line or clear the whole window */
//    c  << "Press enter to delete this line: ";
//    std::cin.get();
//    c.eraseLine();
//    std::cin.get();
//    c.clear();
//}
//
//void TestColor()
//{
//    Console c;
//    std::array test{
//        to_text(20, 100, 50),       //Green
//        to_text(200, 0, 0),         //RED
//        to_text(100, 100, 100),     //WHITE
//        to_text(0, 100, 100),       //CYAN
//        to_text(0, 100, 0),         //Green
//        to_text(100,0,100),          //MAGANTA
//        to_text(20,50,150),          //BLUE
//        to_text(100,120,0)          //YELLOW
//    };
//    ConsoleEngine eng{ c };
//    for (auto i = 0; i < test.size(); ++i)
//        eng.buffer(i+2, 1) = test[i];
//    eng.draw();
//}
//
void VideoTest()
{
    Console c;
    ConsoleEngine eng{ c, true };
    VideoEngine video{ eng };
    video.load("test.mp4");
    video.play(30);
}
//
//void PictureTest()
//{
//    Console c;
//    ConsoleEngine eng{ c };
//    std::cout << "Console size: " << eng.width << " * " << eng.height;
//    cv::Mat frame=cv::imread("test.jpg");
//    cv::Mat resized;
//    cv::resize(frame, resized, { eng.width, eng.height });
//    cv::namedWindow("test");
//    cv::moveWindow("test", 200, 200);
//    std::cin.get();
//    for (auto i = 0; i < eng.height; ++i)
//    {
//        for (auto j = 0; j < eng.width; ++j)
//        {
//            auto pixel = resized.at<cv::Vec3b>(i, j);
//            eng.buffer(i, j) = to_text(pixel[2], pixel[1], pixel[0]);   //Opencv stores pixel in (BGR) format
//        }
//    }
//    eng.drawW();
//    cv::destroyAllWindows();
//}
//
//void TimerTest()
//{
//    int i = 0;
//    while (true)
//    {
//        ScopedTimer t{ 30 };
//        std::cout << i++ << '\n';
//    }
//}

int main(int argc, char** argv)
{
    VideoTest();
}
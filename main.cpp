#include "TextVideo.h"
#include <iostream>
#include "Shape.h"
#include "Console.h"
#include "ConsoleEngine.h"
#include <thread>
#include <atomic>

const std::string currentDir = "../../../test/";


void TestEngine()
{
    console.printConsoleInfo();
    std::cin.get();
    ConsoleEngine eng{console};
    auto [left, right] = eng.add<RectangleArea>().divide(RectangleArea::Divisor::Vertical, 0);
    auto [upper_left, bottom_left] = left.divide(RectangleArea::Divisor::Horizontal, 0);
    auto [upper_right, bottom_right] = right.divide(RectangleArea::Divisor::Horizontal, 0);
    
    auto upper_left_bar = upper_left.add<ProgressBar>(); 
    upper_left_bar.setPercentage(20); 
    upper_left_bar.setColor(Color::RED);
    /*auto upper_right_bar    =*/ upper_right.add<ProgressBar>().setPercentage(30);
    /*auto bottom_left_bar    =*/ bottom_left.add<ProgressBar>().setPercentage(40);
    /*auto bottom_right_bar   =*/ bottom_right.add<ProgressBar>().setPercentage(50);
    

    eng.draw();
    std::cin.get();
}

void TestConsole()
{

    /*Setting text color using familiar syntax */
    console << Color::RED << "This is a red text on black background\n";
    /*Or use traditional member function*/
    console.set(Color::MAGENTA).writeln("This is magenta text");

    /*Setting background color using the same syntax */
    console << BackgroundColor::WHITE << "This is a magenta text on white background\n";
    console.set(BackgroundColor::DEFAULT, false).set(Color::WHITE);

    /*Moving cursor using relative or absolute position */
    console.moveCursor(4, Direction::RIGHT);
    console << "Indent text\n";
    console.moveCursorTo({ 20,10 });
    console << "Text start at [20,10]\n";
    console.moveCursorTo(MIDDLE{});
    
    /*Erase one line or clear the whole window */
    console  << "Press enter to delete this line: ";
    std::cin.get();
    console.eraseLine();
    std::cin.get();
    console.clear();
}

//void TestColor()
//{
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
//    ConsoleEngine eng{ console };
//    for (auto i = 0; i < test.size(); ++i)
//        eng.buffer(i+2, 1) = test[i];
//    eng.draw();
//}

void VideoTestSingle()
{
    const std::string file = "test.mp4";
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/
    
    auto frame = engine.add<Video>();
    frame.load(currentDir + file);
    /*windows cmd doesn't like .. so I copied it to the build directory*/
    system(file.c_str());
    frame.play(30);
    
    //TEST only
    //frame.drawColorWall();
}

void VideoTestDual()
{
    console.printConsoleInfo();
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/
    auto frame = engine.add<RectangleArea>();

    auto [videoFrameLeft, videoFrameRight] = frame.divide(RectangleArea::Divisor::Vertical, 0);
    auto v_left = videoFrameLeft.add<Video>(engine);
    auto v_right = videoFrameRight.add<Video>(engine);

    //v_UpLeft.load("test.mp4");
    //v_UpRight.load("test.mp4");
    //v_DownLeft.load("test.mp4");
    //v_DownRight.load("test.mp4");
    v_left.load(currentDir + "test.mp4");
    v_right.load(currentDir + "test2.mp4");
    VideoEngine videos{ std::vector{&v_left, &v_right} };
}

void VideoTestQuad()
{
    ConsoleEngine engine{ console };
    engine.setWcharMode();
    /*Play a single video*/
    /*Or use multithreading to play multiple videos at once*/
    auto frame = engine.add<RectangleArea>();

    auto [videoFrameLeft, videoFrameRight] = frame.divide(RectangleArea::Divisor::Vertical, 0);
    auto [upLeft, downLeft] = videoFrameLeft.divide(RectangleArea::Divisor::Horizontal, 0);
    auto [upRight, downRight] = videoFrameRight.divide(RectangleArea::Divisor::Horizontal, 0);
    
    auto v_UpLeft = upLeft.add<Video>(engine);
    auto v_DownLeft = downLeft.add<Video>(engine);
    auto v_UpRight = upRight.add<Video>(engine);
    auto v_DownRight = downRight.add<Video>(engine);

    //v_UpLeft.load("test.mp4");
    //v_UpRight.load("test.mp4");
    //v_DownLeft.load("test.mp4");
    //v_DownRight.load("test.mp4");
    v_UpLeft.load(currentDir+"test.mp4");
    v_UpRight.load(currentDir + "test2.mkv");
    v_DownLeft.load(currentDir + "test3.mkv");
    v_DownRight.load(currentDir + "test4.mkv");
    VideoEngine videos{ std::vector{&v_UpLeft, &v_DownLeft, &v_UpRight, &v_DownRight} };
}

void PictureTest()
{
    const std::string file = "test.jpg";
    Console console;
    ConsoleEngine engine{ console };
    engine.setWcharMode();

    auto frame = engine.add<PictureEngine>();
    frame.load(currentDir + file);
}


void ScrollViewTest()
{
    ConsoleEngine eng{ console };
    auto canvas = eng.add<RectangleArea>();
    canvas.setBorder('U', 'D', 'L', 'R');
    auto [left, right] = canvas.divide(RectangleArea::Divisor::Vertical, 0);
    left.setBorder('1', '2', '3', '4');
    auto area = left.add<ScrollView>();

    /*add text -> area*/
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("Hello world1");
    area.push("A long text that needs two line to display ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    area.push("Hello world1");

    area.draw();

    eng.draw();
}
int main(int argc, char** argv)
{
    VideoTestSingle();
    //PictureTest();
    //console.printConsoleInfo();
}
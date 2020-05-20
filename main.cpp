#include <iostream>
#include "Shape.h"
#include "Console.h"
#include "ConsoleEngine.h"
#include <thread>
#include <atomic>

static std::atomic<float> elapsed_time{};

void PrintFps()
{
    while(true)
    {
        SetConsoleTitleA((std::string{"FPS="}+std::to_string(1.0f / (0.000000001 * elapsed_time))).data());
    }
}

void TestEngine()
{
    Console console;
    console.printConsoleInfo();
    std::cin.get();
    ConsoleEngine eng{console};
    auto [left, right] = eng.add<RectangleArea>().divide(RectangleArea::Divisor::Vertical, 0);
    auto [upper_left, bottom_left] = left.divide(RectangleArea::Divisor::Horizontal, 0);
    auto [upper_right, bottom_right] = right.divide(RectangleArea::Divisor::Horizontal, 0);
    
    /*auto upper_left_bar     =*/ upper_left.add<ProgressBar>().setPercentage(20);
    /*auto upper_right_bar    =*/ upper_right.add<ProgressBar>().setPercentage(30);
    /*auto bottom_left_bar    =*/ bottom_left.add<ProgressBar>().setPercentage(40);
    /*auto bottom_right_bar   =*/ bottom_right.add<ProgressBar>().setPercentage(50);
    

    eng.draw();
    std::cin.get();
}

void TestConsole()
{
    /*Construct a new [Console] class */
    Console c;

    /*Setting text color using familiar syntax */
    c << Color::RED << "This is a red text on black background\n";
    /*Or use traditional member function*/
    c.set(Color::MAGENTA).writeln("This is magenta text");

    /*Setting background color using the same syntax */
    c << BackgroundColor::WHITE << "This is a magenta text on white background\n";
    c.set(BackgroundColor::DEFAULT, false).set(Color::WHITE);

    /*Moving cursor using relative or absolute position */
    c.moveCursor(4, Direction::RIGHT);
    c << "Indent text\n";
    c.moveCursorTo({ 20,10 });
    c << "Text start at [20,10]\n";
    c.moveCursorTo(MIDDLE{});
    
    /*Erase one line or clear the whole window */
    c  << "Press enter to delete this line: ";
    std::cin.get();
    c.eraseLine();
    std::cin.get();
    c.clear();
}

int main()
{
    TestConsole();
}
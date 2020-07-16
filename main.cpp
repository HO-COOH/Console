//#include "TextVideo.h"
//#include <iostream>
//#include "Shape.h"
//#include "Console.h"
//#include "ConsoleEngine.h"
//#include <thread>
//#include <atomic>
//
//
//void TestEngine()
//{
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
//
//    /*Setting text color using familiar syntax */
//    console << Color::RED << "This is a red text on black background\n";
//    /*Or use traditional member function*/
//    console.set(Color::MAGENTA).writeln("This is magenta text");
//
//    /*Setting background color using the same syntax */
//    console << BackgroundColor::WHITE << "This is a magenta text on white background\n";
//    console.set(BackgroundColor::DEFAULT, false).set(Color::WHITE);
//
//    /*Moving cursor using relative or absolute position */
//    console.moveCursor(4, Direction::RIGHT);
//    console << "Indent text\n";
//    console.moveCursorTo({ 20,10 });
//    console << "Text start at [20,10]\n";
//    console.moveCursorTo(MIDDLE{});
//
//    /*Erase one line or clear the whole window */
//    console  << "Press enter to delete this line: ";
//    std::cin.get();
//    console.eraseLine();
//    std::cin.get();
//    console.clear();
//}

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

//void ScrollViewTest()
//{
//    ConsoleEngine eng{ console };
//    auto canvas = eng.add<RectangleArea>();
//    canvas.setBorder('U', 'D', 'L', 'R');
//    auto [left, right] = canvas.divide(RectangleArea::Divisor::Vertical, 0);
//    left.setBorder('1', '2', '3', '4');
//    auto area = left.add<ScrollView>();
//
//    /*add text -> area*/
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("Hello world1");
//    area.push("A long text that needs two line to display ABCDEFGHIJKLMNOPQRSTUVWXYZ");
//    area.push("Hello world1");
//
//    area.draw();
//
//    eng.draw();
//}


#include "Console.h"
#include <iostream>

#ifdef LINUX
void test()
{
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_GREEN);
    attron(COLOR_PAIR(1));
    printw("RED text + Green back\n");
    refresh();
    //attroff(COLOR_PAIR(1));
    init_pair(2, COLOR_GREEN, COLOR_WHITE);
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    printw("Green text + white back\n");
    refresh();
    getch();
    endwin();
}
#endif

void test2()
{
    console.printConsoleInfo();
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
    console << "Press enter to delete this line: ";
    std::cin.get();
    console.eraseLine();
    std::cin.get();
    console.clear();
}
int main(int argc, char *argv[])
{
    //test();

    test2();
}



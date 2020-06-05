# Console
 Simple Windows Console wrapper with easy-to-use APIs and drawing functionality. The project is focused on sweet syntax and expressive code.
## Description
The original implementation was used in [Simple TCP chatroom Client](https://github.com/HO-COOH/CS7850/tree/master/Project/V2). And as I was developing that project, I found out that just wrapping up the native Win32 console APIs is far from enough. Therefore I make it as a new project. 
## How to use
Compile time polymorphism is given higher priority. But I tried to make it easy to use.

In the future, runtime polymorphism will be considered.

An example of using ``Console`` class.
```cpp
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
```

```cpp
#include "Console.h"        //For simple Windows console wrapper
#include "ConsoleEngine.h"  //For Console drawing engine
int main()
{
    Console console;            //create a console window
    ConsoleEngine eng{console}; //create a console engine, bind with the just-created window

    auto canvas=eng.add<RectangleArea>();   //create a rectangleArea, returns a handle to the area

    canvas.setBorder('U', 'D', 'L', 'R');   //set the border line characters in the order of: up, down, left, right

    /*Divide a area into 2 (with an optional [position] parameter), returning a pair of handles*/
    //if [position] is set to 0, then automatically divide in the middle
    auto [chatArea, userArea]=canvas.divide(RectangleArea::Divisor::Vertical, 30);

    /*Use of TextBox class */
    auto chatView=chatArea.add<TextBox>();
    chatView.setText("Hello World");
}
```

## Modules
The project consists of these following easy-to-use elements. 
### Console
You already seen above.
### ConsoleEngine
You already seen above.
### GUI Elements
All GUI elements are inherited from ``RectangleArea``.
- TextBox
- ScrollArea
- ProgressBar

GUI library is written with code reusability in mind. They work great not only with ``ConsoleEngine``, but also with your ordinary ``std::cout`` or ``std::wcout``, meaning that you can reuse them without engaging with ``ConsoleEngine`` in your own project, freeing you from re-implementing basic GUI elements in the Console.
```cpp
#include "Shape.h"
ProgressBar bar{20, 1};
bar.setPercentage(30);
std::cout << bar << '\n';
```

### VideoEngine
```cpp
#include <ConsoleEngine.h>
#include <VideoEngine.h>
void VideoTest()
{
    Console console;
    ConsoleEngine engine{console};
    /*Play a single video*/
    {
        auto video = engine.add<VideoEngine>();
        video.load("SomeFile.mp4");
        video.play();
    }
    /*Or use multithreading to play multiple videos at once*/
    {
        auto [videoFrameLeft, videoFrameRight]=engine.divide(RectangleArea::Vertical);
        auto videoLeft = videoFrameLeft.add<VideoEngine>();
        auto videoRight = videoDrameRight.add<VideoEngine>();
        videoLeft.load("SomeFile.mp4");
        videoRight.load("AnotherFile.mp4");
        std::thread t[2]{videoLeft.play(), videoRAight.play()};
        for(auto& thread: t)
            thread.join();
    }
}
```
- A single-video-single-window demo
- A multi-video-single-window demo
### PictureEngine


## Plan To-Do
- One console (std handle) √
- Colors √
- Able to create multiple console windows (so change originally ``static`` functions to ``non-static``)
- Drawing functionality
    + borders √
    + Text region
    + Scroll View (automatic scrolling text region)
    + Progress bar
    + Forms
- Keyboard events √×(partially)

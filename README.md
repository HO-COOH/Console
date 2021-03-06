# Console

 Simple **Windows & Ncurses** Console wrapper with easy-to-use APIs and drawing functionality. The project is focused on sweet syntax, without ugly types and pointers nonsense.

**!Still very early in development!**

## Description

The original implementation was used in [Simple TCP chatroom Client](https://github.com/HO-COOH/CS7850/tree/master/Project/V2). And as I was developing that project, I found out that just wrapping up the native Win32 console APIs is far from enough. Therefore I make it as a new project. 

## How to use

Compile time polymorphism is given higher priority. But I tried to make it easy to use.

**``console`` is a global object that is guaranteed to initialized first before you use, so no need to construct it anymore if you only use one console window**

In the future, runtime polymorphism will be considered.

An example of using ``Console`` class.

```cpp
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
    console << "Press enter to delete this line: ";
    std::cin.get();
    console.eraseLine();
    std::cin.get();
    console.clear();
}
```

```cpp
#include "Console.h"        //For simple Windows console wrapper
#include "ConsoleEngine.h"  //For Console drawing engine
int main()
{
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

### Console

Windows ConsoleAPI & ncurses wrapper. ``./include/Console.h``

### ConsoleEngine

Take ownership of the console and draw stuff. ``./include/ConsoleEngine.h``

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
    ConsoleEngine engine{console};
    /*Play a single video*/
    {
        auto video = engine.add<Video>();
        video.load("SomeFile.mp4");
        video.play(30);//play at 30 fps
    }
    /*Or use multithreading to play multiple videos at once*/
    {
        auto [videoFrameLeft, videoFrameRight]=engine.divide(RectangleArea::Vertical);
        auto videoLeft = videoFrameLeft.add<Video>(engine);
        auto videoRight = videoDrameRight.add<Video>(engine);
        videoLeft.load("SomeFile.mp4");
        videoRight.load("AnotherFile.mp4");
        VideoEngine{ {videoLeft, videoRight} };//play 2 videos
    }
}
```

- A single-video-single-window demo
  + GrayScale Video Demo [China](https://www.bilibili.com/video/BV1aK411p7hJ) [Non China](https://www.youtube.com/watch?v=0b1pUuar8Fc&t=151s)
  + Colored Video Demo [China](https://www.bilibili.com/video/BV165411Y7w6) [Non China](https://www.youtube.com/watch?v=9k3mMGZGRz4)
- A multi-video-single-window demo
  
  ### PictureEngine

## Plan To-Do

- One console (std handle) √
- Colors √
- Able to create multiple console windows~~(so change originally ``static`` functions to ``non-static``)~~
- Drawing functionality
  + borders √
  + Text region
  + Scroll View (automatic scrolling text region)
  + Progress bar
  + Forms
- Keyboard events √×(partially)

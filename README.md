# Console
 Simple Windows Console wrapper with easy-to-use APIs and drawing functionality. The project is focused on sweet syntax and expressive code.
## Description
The original implementation was used in [Simple TCP chatroom Client](https://github.com/HO-COOH/CS7850/tree/master/Project/V2). And as I was developing that project, I found out that just wrapping up the native Win32 console APIs is far from enough. Therefore I make it as a new project. 
## How to use
Compile time polymorphism is given higher priority. But I tried to make it easy to use.

In the future, runtime polymorphism will be considered.
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

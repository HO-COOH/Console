# Console
 Simple Windows Console wrapper with easy-to-use APIs and drawing functionality
## Description
The original implementation was used in [Simple TCP chatroom Client](https://github.com/HO-COOH/CS7850/tree/master/Project/V2). And as I was developing that project, I found out that just wrapping up the native Win32 console APIs is far from enough. Therefore I make it as a new project. 
## Plan To-Do
- One console (std handle) √
- Colors √
- Able to create multiple console windows (so change originally ``static`` functions to ``non-static``)
- Drawing functionality
    + borders √
    + Text region
    + Progress bar
    + Forms
- Keyboard events √×(partially)

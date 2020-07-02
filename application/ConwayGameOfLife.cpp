#include "Console.h"
#include "ConsoleEngine.h"

class MyLife
{
    ConsoleEngine eng{console};

public:
    MyLife();
    void update();
};
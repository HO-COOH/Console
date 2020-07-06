#include "Console.h"
#include "ConsoleEngine.h"
#include "TimedEvent.hpp"
#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include <array>
#include <functional>


static std::mt19937 rdEng{ std::random_device{}() };
char randomChar()
{
    static std::uniform_int_distribution<int> rdNum{ '!', '}' };
    return rdNum(rdEng);
}

int main(int argc, char** argv)
{
    ConsoleEngine eng{ console };
    eng.setAsciiMode();
    const auto width = console.getWidth();
    const auto height = console.getHeight();
    std::uniform_int_distribution rdNumHeight{ 0, (int)height };
    std::vector<short> head( width, 0);
    std::vector<short> tail( width, 0);
    std::vector<short> length(width, 0);
    std::vector<bool> hasContent(width);

    ScopedTimer t{ 60000 };
    std::vector<short> emptyCol;
    emptyCol.reserve(width);
    while (true)
    {
        /*Get empty columns*/
        for (auto col = 0; col < width; ++col)
        {
            if (!hasContent[col])
                emptyCol.push_back(col);
        }

        /*select some empty columns*/
        if (emptyCol.size() > width/2)
        {
            std::array<short, 10> toFill;
            std::uniform_int_distribution rd{ 0, static_cast<int>(emptyCol.size() - 1) };
            std::generate(toFill.begin(), toFill.end(), [&rd, &emptyCol] {return emptyCol[rd(rdEng)]; });
            for (auto& col : toFill)
            {
                eng.buffer(0, col).Char.AsciiChar = randomChar();
                eng.buffer(0, col).Attributes = static_cast<WORD>(Color::WHITE) | FOREGROUND_INTENSITY;
                hasContent[col] = true;
                head[col] = 1;
                length[col] = rdNumHeight(rdEng);
            }
        }

        
        for (auto i = 0; i < width; ++i)
        {
            /*generate a random char at [head] position*/
            if (hasContent[i])
            {
                if (head[i] != 0)
                {
                    eng.buffer(head[i], i).Char.AsciiChar = randomChar();
                    eng.buffer(head[i] - 1, i).Attributes = static_cast<WORD>(Color::GREEN) | FOREGROUND_INTENSITY;
                    eng.buffer(head[i], i).Attributes = static_cast<WORD>(Color::WHITE) | FOREGROUND_INTENSITY;
                    ++head[i];
                    if (head[i] >= height)
                        head[i] = 0;
                }
                /*delete the char at [tail] position */
                if ( head[i]==0||head[i]-tail[i] >= length[i])
                {
                    eng.buffer(tail[i], i).Char.AsciiChar = ' ';
                    ++tail[i];
                    if (tail[i] >= height)
                    {
                        tail[i] = 0;
                        hasContent[i] = false;
                    }
                }
            }
            

        }
        eng.draw();
        emptyCol.clear();
        t.wait();
    }

}
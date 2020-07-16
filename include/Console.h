#pragma once


#include <unordered_map>

enum class Direction {
    UP, DOWN, LEFT, RIGHT
};

enum MIDDLE {};

enum class FunctionKey {
    UP, DOWN, LEFT, RIGHT, ENTER, SPACE, BACKSPACE, COUNT
};

#ifdef WINDOWS
#include <Windows.h>

enum class Color {
    BLACK = 0,
    RED = FOREGROUND_RED, GREEN = FOREGROUND_GREEN, BLUE = FOREGROUND_BLUE,
    YELLOW = FOREGROUND_GREEN | FOREGROUND_RED, CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN, MAGENTA = FOREGROUND_BLUE | FOREGROUND_RED,
    WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    NOCHANGE
};
//
//struct Color
//{
//    WORD value;
//    static inline Color BLACK{2};
//};

enum class BackgroundColor {
    RED = BACKGROUND_RED, GREEN = BACKGROUND_GREEN, BLUE = BACKGROUND_BLUE,
    YELLOW = BACKGROUND_GREEN | BACKGROUND_RED, CYAN = BACKGROUND_BLUE | BACKGROUND_GREEN, MAGENTA = BACKGROUND_BLUE | BACKGROUND_RED,
    WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    DEFAULT = 0,
    NOCHANGE
};

inline std::unordered_map<FunctionKey, int> KeyCode {
    {FunctionKey::UP, VK_UP},
    {FunctionKey::DOWN, VK_DOWN},
    {FunctionKey::LEFT, VK_LEFT},
    {FunctionKey::RIGHT, VK_RIGHT},
    {FunctionKey::ENTER, VK_RETURN},
    {FunctionKey::SPACE, VK_SPACE},
    {FunctionKey::BACKSPACE, VK_BACK}
};
#else //Linux

#include <ncurses.h>
#include <sstream>
#include <utility>
#define WORD int
#define HANDLE WINDOW*
enum class Color
{
    BLACK = COLOR_BLACK,
    RED=COLOR_RED, GREEN=COLOR_GREEN, BLUE=COLOR_BLUE,
    YELLOW=COLOR_YELLOW, CYAN=COLOR_CYAN, MAGENTA=COLOR_MAGENTA,
    WHITE=COLOR_WHITE,
    NOCHANGE
};
enum class BackgroundColor
{
    RED = COLOR_RED,
    GREEN = COLOR_GREEN,
    BLUE = COLOR_BLUE,
    YELLOW = COLOR_YELLOW,
    CYAN = COLOR_CYAN,
    MAGENTA = COLOR_MAGENTA,
    WHITE = COLOR_WHITE,
    DEFAULT = COLOR_BLACK,
    NOCHANGE
};
struct COORD
{
    int x;
    int y;
};
struct CHAR_INFO
{
    union {
        char AsciiChar;
        wchar_t UnicodeChar;
    }Char;
    WORD Attributes;
};

namespace std
{
    template<>
    struct hash<std::pair<Color, BackgroundColor>>
    {
        size_t operator()(std::pair<Color, BackgroundColor> const& key) const
        {
            return static_cast<size_t>(key.first)<<8 | static_cast<size_t>(key.second);
        }
    };
}

#endif

#include <iostream>
#include <string>
#include <exception>
#include <string_view>

#define LOG(X) ;



enum class Shade:wchar_t {
    Full = 0x2588,      //75-100
    Dark = 0x2593,      //50-75
    Medium = 0x2592,    //25-50
    Light = 0x2591,     //0-25
    None = ' ',
};


class Console
{     
    static inline int count = 0;
    bool is_std;
    HANDLE hTerminal;
    short width;
    short height;
    WORD default_attrib;
    std::pair<BackgroundColor, bool> current_background_color;
    std::pair<Color, bool> current_foreground_color;

    void setColor();
    void sendPipe(std::string_view msg) const;
    void constructSecondaryConsole();
    void constructStdConsole();


#ifdef LINUX
    /*
        This is a color_pair index cache for ncurses
        Due to the limitation, if you do:
            initscr();
            start_color();
            init_pair(1, COLOR_RED, COLOR_GREEN);
            attron(COLOR_PAIR(1));
            printw("Red text + Green back\n");      //
            refresh();
            attroff(COLOR_PAIR(1));
            init_pair(1, COLOR_GREEN, COLOR_WHITE);
            attron(COLOR_PAIR(1));
            printw("Green text + white back\n");    //this will change the previously printed text too
        And you can't just incrementing the color index every time to change the current color,
        because there will be a maximum limit of number of color pairs

        So that each time a new color_pair is created, it needs to be added to this cache, without constantly creating new pair of colors
        And a color index can be retrieved with: colorIndex[{Color::RED, BackgroundColor::WHITE}]
        And use the existing color pair with: attron(COLOR_PAIR(colorIndex[{Color::RED, BackgroundColor::WHITE}];
     */
    std::unordered_map<std::pair<Color, BackgroundColor>, int> colorIndex;
#endif

public:

#ifdef WINDOWS
    class ConsoleException:public std::runtime_error
    {
    public:
        ConsoleException(std::string const& msg):std::runtime_error(msg+" Code: "+ std::to_string(GetLastError())){}
    };
#else
#endif

    /*Console configuration functions*/
    Console();
    Console& resetConsole(short width, short height, short fontWidth, short fontHeight);
    void printConsoleInfo();
    auto getWidth() const { return width; }
    auto getHeight() const { return height; }


    /*Cursor functions*/
    void backspace(short count=1);
    void moveCursor(short count, Direction direction);
    void moveCursorTo(COORD position);
    void moveCursorTo(MIDDLE);
    COORD getCursorPos() const;
    void hideCursor();
    void showCursor();

    /*Delete content functions*/
    void eraseLine();
    void deleteTo(COORD position);
    void clear();
    void close();

    /*Read console functions*/
    template<typename T>
    T read(const char* prompt = nullptr, bool reprompt = true);

    /*Write console functions*/
    Console& operator<<(Color color);
    Console& operator<<(BackgroundColor bgColor);
    Console& set(Color color=Color::NOCHANGE, bool intensify=true);
    Console& set(std::pair<Color, bool> color);
    Console& set(BackgroundColor bgColor=BackgroundColor::NOCHANGE, bool intensify=false);
    Console& set(std::pair<BackgroundColor, bool> bgColor);
    void changeWindowColor(BackgroundColor bgColor);

    template <typename T>
    Console& operator<<(T&& arg);

    template<typename T>
    Console& writeln(const T& arg, Color color=Color::NOCHANGE);
    Console& putchar(char c, size_t count = 1);
    
    /*Assitive functions*/
    FunctionKey getKey();
    
    
    /*Console manipulation functions, inlined for higher performance*/
    void setTitle(std::string const& title);
    void fillConsole(CHAR_INFO* buffer);
    void fillConsoleW(CHAR_INFO* buffer);
    void beep(int frequency = 800, int duration = 200) const;

    ~Console();
};


inline void Console::setColor()
{
#ifdef WINDOWS
    WORD attrib = static_cast<WORD>(current_foreground_color.first) | static_cast<WORD>(current_background_color.first);
    if (current_foreground_color.second)
        attrib |= FOREGROUND_INTENSITY;
    if (current_background_color.second)
        attrib |= BACKGROUND_INTENSITY;
    if (!SetConsoleTextAttribute(hTerminal, attrib))
        LOG("SetConsoleTextAttributeError");
#else
    static int i = 1;
    if (auto iter = colorIndex.find({ current_foreground_color.first, current_background_color.first }); iter != colorIndex.end())
        attron(COLOR_PAIR(iter->second));
    else
    {
        colorIndex[{current_foreground_color.first, current_background_color.first}] = i;
        init_pair(i, static_cast<short>(current_foreground_color.first), static_cast<short>(current_background_color.first));
        attron(COLOR_PAIR(i));
        i++;
    }
    if (current_foreground_color.second)
        attron(A_BOLD);
#endif
}


inline void Console::sendPipe(std::string_view msg) const
{
#ifdef WINDOWS
    std::cout << "Sending \"" << msg << "\" -> console 2.\n";
    DWORD bytes_written{};
    if (!WriteFile(hTerminal, msg.data(), msg.length(), &bytes_written, nullptr))
    {
        puts("Test writing failed");
        CloseHandle(hTerminal);
    }
#else
    //TODO:
#endif
}


inline Console& Console::operator<<(Color color)
{
    set(color);
    return *this;
}


inline Console& Console::operator<<(BackgroundColor bgColor)
{
    set(bgColor);
    return *this;
}

template <typename T>
Console& Console::operator<<(T&& arg)
{
#ifdef WINDOWS
    std::cout << arg;
#else
    std::stringstream ss;
    ss << arg;
    printw(ss.str().c_str());
    refresh();
#endif
    return *this;
}

inline void Console::moveCursorTo(COORD position)
{
#ifdef WINDOWS
    if (!SetConsoleCursorPosition(hTerminal, position))
        LOG("Move cursor to error!\n");
#else
    if(!move(position.y, position.x)!=OK)
        LOG("Move cursor to error!\n");
#endif
}
 

inline void Console::close()
{
#ifdef WINDOWS
    PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
#else
    endwin();
#endif
}

inline Console& Console::putchar(char c, size_t count)
{
#ifdef WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    //DWORD count;	//If the pointer to the last parameter is set to nullptr, there will be an exception
#else
    while(count--)
        addch(c);
#endif
    return *this;
}


inline void Console::setTitle(std::string const& title)
{
#ifdef WINDOWS
    //if succeeds, the return value is NON 0
    if(!SetConsoleTitleA(title.c_str()))
        LOG("Setting title error");
#else
    /*
        May NOT work with all terminals.
        Must use fflush() during a ncurses session otherwise it will not have effect until ncurses session ended.
    */
    std::cout << "\033]0;" << title << "\007";
    fflush(stdout);
#endif
}

inline void Console::fillConsole(CHAR_INFO* buffer)
{
#ifdef WINDOWS
    SMALL_RECT written_region{ 0,0,width - 1, height - 1 };
    if (WriteConsoleOutputA(hTerminal, buffer, { width, height }, { 0,0 }, &written_region) == 0)
        std::cerr << "WriteConsoleOutput error, write: (" << written_region.Left << " ," << written_region.Top << ") -> (" << written_region.Right << " ," << written_region.Bottom << ")\n";
#else
    auto pos = getCursorPos();
    for (auto row = 0; row < height; ++row)
    {
        for (auto col = 0; col < width; ++col)
        {
            mvaddch(row, col, buffer->Char.AsciiChar);
            ++buffer;
        }
    }
    moveCursorTo(pos);
    refresh();
#endif
}

inline void Console::fillConsoleW(CHAR_INFO* buffer)
{
#ifdef WINDOWS
    SMALL_RECT written_region{ 0,0,width - 1, height - 1 };
    if (WriteConsoleOutputW(hTerminal, buffer, { width, height }, { 0,0 }, &written_region) == 0)
        std::cerr << "WriteConsoleOutput error, write: (" << "Get console screen buffer info failed!\n";
#else
    auto pos = getCursorPos();
    for (auto row = 0; row < height; ++row)
    {
        for (auto col = 0; col < width; ++col)
        {
            mvaddch(row, col, buffer->Char.UnicodeChar);
            ++buffer;
        }
    }
    moveCursorTo(pos);
    refresh();
#endif
}

inline void Console::beep(int frequency, int duration) const
{
#ifdef WINDOWS
    Beep(frequency, duration);
#else
    /*

    */
#endif
}

template<typename T>
T Console::read(const char* prompt, bool reprompt)
{
#ifdef WINDOWS
    T i;
    do {
        if(prompt)
            std::cout << prompt;
        auto pos = getCursorPos();
        if (!std::cin)
        {
            std::cin.clear();
            while (std::cin.get() != '\n')
                ;
            deleteTo(pos);
        }
    } while (!(std::cin >> i));
    return i;
#else
    T i;
    do{
        if(prompt)
            printw(prompt);
        auto pos = getCursorPos();
        std::stringstream ss;
        char c;
        while ((c = getch()) != '\n')
            ss << c;
        ss >> i;
        if (ss)
            break;
    } while (true);
#endif
}

template<typename T>
Console& Console::writeln(const T& arg, Color color)
{
#ifdef WINDOWS
    if (color == Color::NOCHANGE)
        std::cout << arg << '\n';
    else
    {
        if (CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info; GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
        {
            WORD old_color = screen_buffer_info.wAttributes;
            set(color);
            std::cout << arg << '\n';
            if (SetConsoleTextAttribute(hTerminal, old_color) == 0)
                std::cerr << "Set console text attribute error!\n";
        }
        else
            std::cerr << "Get console screen buffer info failed!\n";
    }
#else
    if (color == Color::NOCHANGE)
        *this << arg << '\n';
    else
    {
        auto oldColor = current_foreground_color;
        set(color);
        *this << arg << '\n';
        set(oldColor);
    }
    refresh();
#endif
    return *this;
}



/*
    The following code is the nifty-counter idiom applied for constructing a global console object.
*/
extern class Console& console;
class ConsoleInitializer
{
    static unsigned count;
    static void init();
    static void cleanUp();
public:
    ConsoleInitializer();
    ConsoleInitializer(ConsoleInitializer const&) = delete;
    ConsoleInitializer(ConsoleInitializer&&) = delete;
    ConsoleInitializer& operator=(ConsoleInitializer const&) = delete;
    ConsoleInitializer& operator=(ConsoleInitializer&&) = delete;
    ~ConsoleInitializer();
};
static ConsoleInitializer consoleInitializer;
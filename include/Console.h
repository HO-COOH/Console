#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <exception>
#include <unordered_map>


#define LOG(X) ;


enum class Color {
    BLACK = 0,
    RED = FOREGROUND_RED, GREEN = FOREGROUND_GREEN, BLUE = FOREGROUND_BLUE,
    YELLOW = FOREGROUND_GREEN | FOREGROUND_RED, CYAN = FOREGROUND_BLUE | FOREGROUND_GREEN, MAGENTA = FOREGROUND_BLUE | FOREGROUND_RED,
    WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    NOCHANGE
};

enum class BackgroundColor {
    RED = BACKGROUND_RED, GREEN = BACKGROUND_GREEN, BLUE = BACKGROUND_BLUE,
    YELLOW = BACKGROUND_GREEN | BACKGROUND_RED, CYAN = BACKGROUND_BLUE | BACKGROUND_GREEN, MAGENTA = BACKGROUND_BLUE | BACKGROUND_RED,
    WHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    DEFAULT = 0,
    NOCHANGE
};

enum class Direction {
    UP, DOWN, LEFT, RIGHT
};

enum MIDDLE {};

enum class FunctionKey {
    UP , DOWN , LEFT , RIGHT , ENTER , SPACE , BACKSPACE , COUNT 
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

enum class Shade:wchar_t {
    Full = 0x2588,      //75-100
    Dark = 0x2593,      //50-75
    Medium = 0x2592,    //25-50
    Light = 0x2591,     //0-25
    None = ' ',
};


class Console
{   
private:    
    static inline int count = 0;
    bool is_std;
    HANDLE hTerminal;
    short width;
    short height;
    WORD default_attrib;
    WORD current_background_color;
    WORD current_foreground_color;
    void setColor(WORD color);
    void sendPipe(std::string_view msg) const;
    void constructSecondaryConsole();
    void constructStdConsole();
public:

    class ConsoleException:public std::runtime_error
    {
    public:
        ConsoleException(std::string const& msg):std::runtime_error(msg+" Code: "+ std::to_string(GetLastError())){}
    };

    /*Console configuration functions*/
    Console();
    Console& resetConsole(short width, short height, short fontWidth, short fontHeight);
    void printConsoleInfo() const;
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
    void clear();
    void close();

    /*Write console functions*/
    Console& operator<<(Color color);
    Console& operator<<(BackgroundColor bgColor);
    template <typename T>
    Console &operator<<(T &&arg) { std::cout << arg; return *this; }
    Console& set(Color color=Color::NOCHANGE, bool intensify=true);
    Console& set(BackgroundColor bgColor=BackgroundColor::NOCHANGE, bool intensify=true);
    void changeWindowColor(BackgroundColor bgColor);
    template<typename T>
    Console& writeln(const T& arg, Color color=Color::NOCHANGE);
    Console& putchar(char c, size_t count = 1);
    
    /*Assitive functions*/
    FunctionKey getKey();
    
    
    /*Console manipulation functions, inlined for higher performance*/
    void setTitle(std::string const& title);
    void fillConsole(CHAR_INFO* buffer) const;
    void fillConsoleW(CHAR_INFO* buffer) const;

    ~Console();
};

inline void Console::setColor(WORD color)
{
    if (!SetConsoleTextAttribute(hTerminal, color))
        MessageBox(NULL, TEXT("SetConsoleTextAttributeError"), TEXT("Error"), MB_OK);
}

inline void Console::sendPipe(std::string_view msg) const
{
    std::cout << "Sending \"" << msg << "\" -> console 2.\n";
    DWORD bytes_written{};
    if (!WriteFile(hTerminal, msg.data(), msg.length(), &bytes_written, nullptr))
    {
        puts("Test writting failed");
        CloseHandle(hTerminal);
    }
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


inline void Console::moveCursorTo(COORD position)
{
    if (!SetConsoleCursorPosition(hTerminal, position))
        LOG("Move cursor to error!\n");
}
 

inline void Console::close()
{
    PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

inline Console& Console::putchar(char c, size_t count)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    //DWORD count;	//If the pointer to the last parameter is set to nullptr, there will be an exception
    return *this;
}


inline void Console::setTitle(std::string const& title)
{
    //if succeeds, the return value is NON 0
    if(!SetConsoleTitleA(title.c_str()))
        MessageBox(NULL, TEXT("Setting title error"), TEXT("Error"), MB_OK);
}

inline void Console::fillConsole(CHAR_INFO* buffer) const
{
    SMALL_RECT written_region{ 0,0,width - 1, height - 1 };
    if (WriteConsoleOutputA(hTerminal, buffer, { width, height }, { 0,0 }, &written_region) == 0)
        std::cerr << "WriteConsoleOutput error, write: (" << written_region.Left << " ," << written_region.Top << ") -> (" << written_region.Right << " ," << written_region.Bottom << ")\n";
}

inline void Console::fillConsoleW(CHAR_INFO* buffer) const
{
    SMALL_RECT written_region{ 0,0,width - 1, height - 1 };
    if (WriteConsoleOutputW(hTerminal, buffer, { width, height }, { 0,0 }, &written_region) == 0)
        std::cerr << "WriteConsoleOutput error, write: (" << written_region.Left << " ," << written_region.Top << ") -> (" << written_region.Right << " ," << written_region.Bottom << ")\n";
}

template<typename T>
Console& Console::writeln(const T& arg, Color color)
{
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
    return *this;
}

extern class Console& console;
class ConsoleInitializer
{
    static unsigned count;
    static void init();
    static void cleanUp();
public:
    ConsoleInitializer();
    ~ConsoleInitializer();
};
static ConsoleInitializer consoleInitializer;
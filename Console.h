#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <thread>	//for getKey
#include <chrono>
#include <exception>
#define DEBUG

#ifdef DEBUG
#define LOG(X) {std::cerr<<(X);}
#elif
#define LOG(X) ;
#endif

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

#ifdef UNICODE
enum class Shade {
    Full = 0x2588,
    Dark = 0x2593,
    Medium = 0x2592,
    Light = 0x2591
};
#endif


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

    void setColor(WORD color)
    {
        if (!SetConsoleTextAttribute(hTerminal, color))
            MessageBox(NULL, TEXT("SetConsoleTextAttributeError"), TEXT("Error"), MB_OK);
    }

    void sendPipe(std::string_view msg) const
    {
        std::cout << "Sending \"" << msg << "\" -> console 2.\n";
        DWORD bytes_written{};
        if (!WriteFile(hTerminal, msg.data(), msg.length(), &bytes_written, nullptr))
        {
            puts("Test writting failed");
            CloseHandle(hTerminal);
        }
    }

public:
    class ConsoleException:public std::runtime_error
    {
    public:
        ConsoleException(std::string const& msg):std::runtime_error(msg+" Code: "+ std::to_string(GetLastError())){}
    };
    Console();
    void printConsoleInfo() const;
    Console& set(Color color=Color::NOCHANGE, bool intensify=true);
    Console& set(BackgroundColor bgColor=BackgroundColor::NOCHANGE, bool intensify=true);
    void changeWindowColor(BackgroundColor bgColor);
    Console& operator<<(Color color);
    Console& operator<<(BackgroundColor bgColor);

    template <typename T>
    Console &operator<<(T &&arg) { std::cout << arg; return *this; }

    void backspace(short count=1);
    void moveCursor(short count, Direction direction);
    void moveCursorTo(COORD position);
    void moveCursorTo(MIDDLE);
    COORD getCursorPos() const;
    void eraseLine();
    void clear();
    void close();

    template<typename T>
    Console& writeln(const T& arg, Color color=Color::NOCHANGE);

    Console& putchar(char c, size_t count = 1);

    void hideCursor();
    void showCursor();
    void setTitle(std::string const& title);
    FunctionKey getKey();

    void fillConsole(CHAR_INFO* buffer) const
    {
        SMALL_RECT written_region{ 0,0,width - 1, height - 1 };
        if (WriteConsoleOutputA(hTerminal, buffer, { width, height }, { 0,0 }, &written_region) == 0)
        {
            std::cerr << "WriteConsoleOutput error, write: (" << written_region.Left << " ," << written_region.Top << ") -> (" << written_region.Right << " ," << written_region.Bottom << ")\n";
        }
    }

    auto getWidth() const { return width; }
    auto getHeight() const { return height; }
    ~Console() 
    { 
        SetConsoleTextAttribute(hTerminal, default_attrib);
        CloseHandle(hTerminal);
    }
};

inline Console& Console::set(BackgroundColor bgColor, bool intensify)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return *this;
    current_background_color = static_cast<WORD>(bgColor);
    if (intensify)
        current_background_color |= BACKGROUND_INTENSITY;

    setColor(current_foreground_color | static_cast<WORD>(current_background_color));
    return *this;
}

inline void Console::changeWindowColor(BackgroundColor bgColor)
{
    if (bgColor == BackgroundColor::NOCHANGE)
        return;
    auto length = width * height;
    DWORD actual_written = 0;
    FillConsoleOutputCharacterA(hTerminal, static_cast<CHAR>(bgColor), length, { 0,0 }, &actual_written);
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

inline void Console::backspace(short count)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
    {
        if (SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X - count, screen_buffer_info.dwCursorPosition.Y }) == 0)
            LOG("Set Console Cursor Position failed!\n");
        while (count-- > 0)
            std::cout << ' ';
    }
    else
        LOG("Get console screen buffer info failed!\n");
}

inline void Console::moveCursor(short count, Direction direction)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    if(GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info)!=0)
    {
        bool success = true;
        switch (direction)
        {
        case Direction::UP:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X, screen_buffer_info.dwCursorPosition.Y > count ? screen_buffer_info.dwCursorPosition.Y - count : 0 });
            break;
        case Direction::DOWN:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X, screen_buffer_info.dwCursorPosition.Y + count });
            break;
        case Direction::LEFT:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X > count ? screen_buffer_info.dwCursorPosition.X - count : 0, screen_buffer_info.dwCursorPosition.Y });
            break;
        case Direction::RIGHT:
            success = SetConsoleCursorPosition(hTerminal, { screen_buffer_info.dwCursorPosition.X + count, screen_buffer_info.dwCursorPosition.Y });
            break;
        }
        if (!success)
            LOG("Move cursor error!\n");
    }
    else
        LOG("Get console screen buffer info failed!\n");
}

inline void Console::moveCursorTo(COORD position)
{
    if (!SetConsoleCursorPosition(hTerminal, position))
        LOG("Move cursor to error!\n");
}

inline void Console::moveCursorTo(MIDDLE)
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info);
    SetConsoleCursorPosition(hTerminal, { width / 2, screen_buffer_info.dwCursorPosition.Y });
}

inline COORD Console::getCursorPos() const
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hTerminal, &info);
    return {info.dwCursorPosition.X, info.dwCursorPosition.Y};
}

inline void Console::eraseLine()
{
    CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
    DWORD count;	//If the pointer to the last parameter is set to nullptr, there will be an exception
    if (GetConsoleScreenBufferInfo(hTerminal, &screen_buffer_info) != 0)
    {
        if (screen_buffer_info.dwCursorPosition.X == 0)
        {
            FillConsoleOutputCharacter(hTerminal, ' ', screen_buffer_info.srWindow.Right, { 0, screen_buffer_info.dwCursorPosition.Y - 1 }, &count);//erase the last line
            SetConsoleCursorPosition(hTerminal, { 0, screen_buffer_info.dwCursorPosition.Y - 1 });
        }
        else
        {
            FillConsoleOutputCharacter(hTerminal, ' ', screen_buffer_info.dwCursorPosition.Y + 1, { 0, screen_buffer_info.dwCursorPosition.Y }, &count);//erase current line
            SetConsoleCursorPosition(hTerminal, { 0, screen_buffer_info.dwCursorPosition.Y });
        }
    }
    else
        LOG("Error in erase line!\n");
}

inline void Console::clear()
{
    DWORD actual_written = 0;
    FillConsoleOutputCharacter(hTerminal, ' ', width*height, { 0,0 }, &actual_written);
    SetConsoleCursorPosition(hTerminal, { 0,0 });
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

inline void Console::hideCursor()
{
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = false;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

inline void Console::showCursor()
{
    CONSOLE_CURSOR_INFO prev;
    GetConsoleCursorInfo(hTerminal, &prev);
    prev.bVisible = true;
    if (SetConsoleCursorInfo(hTerminal, &prev) == 0)
        std::cerr << "Hide cursor failed! Code: " << GetLastError() << '\n';
}

inline void Console::setTitle(std::string const& title)
{
    //if succeeds, the return value is NON 0
    if(!SetConsoleTitleA(title.c_str()))
        MessageBox(NULL, TEXT("Setting title error"), TEXT("Error"), MB_OK);
}

inline FunctionKey Console::getKey()
{
    while (true)
    {
        SHORT keys[static_cast<int>(FunctionKey::COUNT)]{};
        for (auto i = 0; i < static_cast<int>(FunctionKey::COUNT); ++i)
        {
            keys[i] = GetAsyncKeyState(KeyCode[static_cast<FunctionKey>(i)]);
            if ((keys[i] & 1) == 1)
                return static_cast<FunctionKey>(i);
        }
        std::this_thread::sleep_for(std::chrono::microseconds{ 10 });
    }
    return FunctionKey::COUNT;
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
